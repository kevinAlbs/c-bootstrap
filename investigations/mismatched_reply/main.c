/*
Test what happens if a thread is cancelled mid-operation, and the client is reused later.
Observed behavior: the reused client may return the response to the previous operation.
In this example, the reused client sends a "ping", but may get the response to the previous "aggregate". Sample output:

Got reply to 'ping': { "cursor" : { "firstBatch" : [ { "_id" : { "$oid" : "66f56257f9b0131bb60e2501" }, "output" : "foo" } ], "id" : 0, "ns" : "db.coll" }, "ok" : 1.0, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1727357527, "i" : 3 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } }, "operationTime" : { "$timestamp" : { "t" : 1727357527, "i" : 3 } } }

*/

#include <mongoc/mongoc.h>
#include <pthread.h>

#define ASSERT(stmt, ...)                                                            \
    if (!(stmt))                                                                     \
    {                                                                                \
        fprintf(stderr, "error at statement \"%s\" on line: %d\n", #stmt, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                                                \
        fprintf(stderr, "\n");                                                       \
        abort();                                                                     \
    }                                                                                \
    else                                                                             \
        (void)0

void *
thrd_fn(void *ctx)
{
    bool ok;
    bson_error_t error;
    mongoc_client_t *client = ctx;

    // Drop collection and insert one document so aggregate sleep() applies exactly once.
    {
        mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");
        mongoc_collection_drop(coll, NULL);
        bson_t empty = BSON_INITIALIZER;
        ok = mongoc_collection_insert_one(coll, &empty, NULL, NULL, &error);
        ASSERT(ok, "error inserting: %s", error.message);
        mongoc_collection_destroy(coll);
    }

    // Sleep for 1 second.
    {
        const char *stalling_cmd_json = BSON_STR({
            "aggregate" : "coll",
            "pipeline" : [ {
                "$project" : {
                    "output" :
                        {"$function" : {"body" : "function() { sleep(1000); return \"foo\"; }", "args" : [], "lang" : "js"}}
                }
            } ],
            "cursor" : {}
        });
        bson_t stalling_cmd;
        ok = bson_init_from_json(&stalling_cmd, stalling_cmd_json, -1, &error);
        ASSERT(ok, "error creating aggregate command: %s", error.message);
        ok = mongoc_client_command_simple(client, "db", &stalling_cmd, NULL, NULL, &error);
        ASSERT(ok, "error sending aggregate command: %s", error.message);
        bson_destroy(&stalling_cmd);
    }
    return NULL;
}

int main()
{
    mongoc_init();

    mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017");

    // Create a thread with a 1 second long operation. Cancel thread before receiving a response.
    {
        pthread_t thrd;
        pthread_create(&thrd, NULL, thrd_fn, client);
        usleep(100 * 1000); // Sleep for 100ms to give time for command to be sent.
        pthread_cancel(thrd);
        pthread_join(thrd, NULL);
    }

    // Reuse client (incorrectly) to send a "ping".
    {
        bson_t *ping_cmd = BCON_NEW("ping", BCON_INT32(1));
        bson_t reply;
        bson_error_t error;

        usleep(900 * 1000); // Sleep for 900ms to give time for "aggregate" to reply.
        bool ok = mongoc_client_command_simple(client, "db", ping_cmd, NULL, &reply, &error);
        ASSERT(ok, "error sending 'ping': %s", error.message);
        printf("Got reply to 'ping': %s\n", bson_as_json(&reply, NULL)); // Sometimes prints aggregate reply!
        bson_destroy(ping_cmd);
        bson_destroy(&reply);
    }

    mongoc_client_destroy(client);

    mongoc_cleanup();
}
