// Check the error returned when a socketTimeoutMS limit is hit.
#include <mongoc/mongoc.h>

static void do_stalling_operation(mongoc_client_t *client)
{
    mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");

    mongoc_collection_drop(coll, NULL);
    bool ok;
    bson_error_t error;
    // Insert one document. Aggregate needs a document to match.
    {
        bson_t *doc = BCON_NEW("foo", "bar");
        ok = mongoc_collection_insert_one(coll, doc, NULL, NULL, &error);
        if (!ok)
        {
            printf("failed to insert: %s\n", error.message);
            abort();
        }
        bson_destroy(doc);
    }

    // Do an aggregate.
    {
        const char *pipeline_str = BSON_STR(
            {
                "pipeline" : [
                    {
                        "$match" : {"foo" : "bar"}
                    },
                    {
                        "$project" : {
                            "output" : {
                                "$function" : {
                                    "body" : "function() { sleep(1000 * 5); return \"foo\"; }",
                                    "args" : [],
                                    "lang" : "js"
                                }
                            }
                        }
                    }
                ]
            });

        bson_t *pipeline = bson_new_from_json((const uint8_t *)pipeline_str, -1, &error);
        if (!pipeline)
        {
            printf("failed to create pipeline: %s\n", error.message);
            abort();
        }

        mongoc_cursor_t *cursor = mongoc_collection_aggregate(coll, MONGOC_QUERY_NONE, pipeline, NULL, NULL);
        const bson_t *doc;
        while (mongoc_cursor_next(cursor, &doc))
        {
            char *doc_str = bson_as_relaxed_extended_json(doc, NULL);
            printf("got doc: %s\n", doc_str);
            bson_free(doc_str);
        }

        if (mongoc_cursor_error(cursor, &error))
        {
            printf("error on cursor: %s\n", error.message);
            abort();
        }

        mongoc_cursor_destroy(cursor);
        bson_destroy(pipeline);
    }

    mongoc_collection_destroy(coll);
}

int main()
{
    mongoc_client_t *client;
    mongoc_init();
    client = mongoc_client_new("mongodb://localhost:27017/?socketTimeoutMS=50");
    do_stalling_operation(client);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}
