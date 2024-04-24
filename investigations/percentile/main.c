// Test using the `$percentile` operator.
// Sample output:
//   Using libmongoc version: 1.22.2
//   got result: { "_id" : null, "test01_percentiles" : [ 67.0 ] }
//
#include <mongoc/mongoc.h>

int main()
{
    mongoc_init();

    bson_error_t error;
    mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017");
    printf("Using libmongoc version: %s\n", mongoc_get_version());

    mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");
    mongoc_collection_drop(coll, NULL); // Ignore error.

    // Insert documents
    {
        // Use `BSON_STR` to make a string literal without quotes.
        const char *docs_json[] = {
            BSON_STR({"studentId" : "2345", "test01" : 62, "test02" : 81, "test03" : 80}),
            BSON_STR({"studentId" : "2356", "test01" : 60, "test02" : 83, "test03" : 79}),
            BSON_STR({"studentId" : "2358", "test01" : 67, "test02" : 82, "test03" : 78}),
            BSON_STR({"studentId" : "2367", "test01" : 64, "test02" : 72, "test03" : 77}),
            BSON_STR({"studentId" : "2369", "test01" : 60, "test02" : 53, "test03" : 72}),
        };

        for (size_t i = 0; i < sizeof(docs_json) / sizeof(docs_json[0]); i++)
        {
            const char *doc_json = docs_json[i];
            bson_t *doc = bson_new_from_json((const uint8_t *)doc_json, -1, NULL);
            BSON_ASSERT(doc);
            bool ok = mongoc_collection_insert_one(coll, doc, NULL, NULL, &error);
            if (!ok)
            {
                printf("Failed to insert document: %s\n", error.message);
                return 1;
            }
            bson_destroy(doc);
        }
    }

    // Aggregate with `$percentile`
    const char *pipeline_json = BSON_STR([
        {
            "$group" : {
                "_id" : null,
                "test01_percentiles" : {
                    "$percentile" : {
                        "input" : "$test01",
                        "p" : [0.95],
                        "method" : "approximate"
                    }
                }
            }
        }
    ]);

    bson_t *pipeline = bson_new_from_json((const uint8_t *)pipeline_json, -1, NULL);
    BSON_ASSERT(pipeline);
    mongoc_cursor_t *cursor = mongoc_collection_aggregate(coll, MONGOC_QUERY_NONE, pipeline, NULL, NULL);
    const bson_t *result;
    while (mongoc_cursor_next(cursor, &result))
    {
        char *result_str = bson_as_relaxed_extended_json(result, NULL);
        printf("Got result: %s\n", result_str);
        bson_free(result_str);
    }

    mongoc_client_destroy(client);
    mongoc_cleanup();
}
