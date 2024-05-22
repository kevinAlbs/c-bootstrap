/*
Runs three threads for 10 seconds. Each thread runs update operations in a loop.

Sample output when running three mongos:

% MONGOC_VERSION=1.25.4 ./run.sh
mongoc_get_version=1.25.4
Connection count before test   : 1
Connection count during test   : 6
Connection count during test   : 6
Connection count during test   : 6
Connection count during test   : 6
Connection count during test   : 6
Connection count during test   : 6
Connection count during test   : 6
Connection count during test   : 6
Connection count during test   : 6
Connection count during test   : 6
10 seconds passed. Stopping test
Connection count after test    : 1

% MONGOC_VERSION=1.16.2 ./run.sh
mongoc_get_version=1.16.2
Connection count before test   : 1
Connection count during test   : 5
Connection count during test   : 5
Connection count during test   : 5
Connection count during test   : 5
Connection count during test   : 5
Connection count during test   : 5
Connection count during test   : 5
Connection count during test   : 5
Connection count during test   : 5
Connection count during test   : 5
10 seconds passed. Stopping test
Connection count after test    : 1
*/

#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// `get_current_connection_count` returns the server reported connection count from one mongos.
static int32_t
get_current_connection_count(void)
{
    const char *mongos_uri = getenv("MONGOS_URI");
    if (!mongos_uri)
    {
        mongos_uri = "mongodb://localhost:27017";
    }
    mongoc_client_t *client = mongoc_client_new(mongos_uri);
    bson_t *cmd = BCON_NEW("serverStatus", BCON_INT32(1));
    bson_t reply;
    bson_error_t error;
    bool ok = mongoc_client_command_simple(client, "admin", cmd, NULL, &reply, &error);
    if (!ok)
    {
        printf("serverStatus failed: %s\n", error.message);
        abort();
    }
    int32_t conns;
    // Get `connections.current` from the reply.
    {
        bson_iter_t iter;
        BSON_ASSERT(bson_iter_init_find(&iter, &reply, "connections"));
        BSON_ASSERT(bson_iter_recurse(&iter, &iter));
        BSON_ASSERT(bson_iter_find(&iter, "current"));
        conns = bson_iter_int32(&iter);
    }
    bson_destroy(&reply);
    bson_destroy(cmd);
    mongoc_client_destroy(client);
    return conns;
}

static volatile int is_running;

// `worker_thread` repeatedly pops a client, runs an update, and pushes the client.
void *worker_thread(void *ctx)
{
    mongoc_client_pool_t *pool = (mongoc_client_pool_t *)ctx;

    while (is_running)
    {
        // Pop client.
        mongoc_client_t *client = mongoc_client_pool_pop(pool);
        mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");

        // Update.
        bson_t *filter = bson_new();
        bson_t *update = BCON_NEW("$set", "{", "foo", "bar", "}");
        bson_error_t error;
        bool ok = mongoc_collection_update_one(coll, filter, update, NULL, NULL, &error);
        if (!ok)
        {
            printf("update failed: %s\n", error.message);
            abort();
        }

        bson_destroy(update);
        bson_destroy(filter);
        mongoc_collection_destroy(coll);
        mongoc_client_pool_push(pool, client); // Push client.
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    mongoc_init();

    printf("mongoc_get_version=%s\n", mongoc_get_version());
    printf("Connection count before test   : %" PRId32 "\n", get_current_connection_count());

    const char *uri_str = getenv("MONGODB_URI");
    if (!uri_str)
    {
        uri_str = "mongodb://localhost:27017/?maxPoolSize=64";
    }

    // Create a client pool to multiple mongoses.
    mongoc_uri_t *uri = mongoc_uri_new(uri_str);
    mongoc_client_pool_t *pool = mongoc_client_pool_new(uri);

    // Start three threads running update operations in a loop.
    is_running = 1;
    pthread_t workers[3];
    BSON_ASSERT(0 == pthread_create(&workers[0], NULL, worker_thread, pool));
    BSON_ASSERT(0 == pthread_create(&workers[1], NULL, worker_thread, pool));
    BSON_ASSERT(0 == pthread_create(&workers[2], NULL, worker_thread, pool));

    // Print the connection count every second.
    int64_t start = bson_get_monotonic_time();
    while (true)
    {
        sleep(1);
        printf("Connection count during test   : %" PRId32 "\n", get_current_connection_count());

        int64_t now = bson_get_monotonic_time();
        if (now - start > 10 * 1000 * 1000)
        {
            printf("10 seconds passed. Stopping test\n");
            is_running = 0; // Should use an atomic.
            break;
        }
    }

    // Join threads.
    BSON_ASSERT(0 == pthread_join(workers[0], NULL));
    BSON_ASSERT(0 == pthread_join(workers[1], NULL));
    BSON_ASSERT(0 == pthread_join(workers[2], NULL));

    mongoc_client_pool_destroy(pool);

    // Sleep for five seconds for server to update connection count.
    // The interrupted monitoring connection appears to be reported for a short duration after closing.
    sleep(5);

    printf("Connection count after test    : %" PRId32 "\n", get_current_connection_count());

    mongoc_cleanup();
    return EXIT_SUCCESS;
}
