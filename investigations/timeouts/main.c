#include <mongoc/mongoc.h>

int main()
{
    mongoc_client_t *client;
    char *uristr = "mongodb://localhost:27017";
    bson_error_t error;

    mongoc_init();
    client = mongoc_client_new(uristr);

    mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");

    // Set a `maxTimeMS` of 1 second on `insert`:
    {
        bson_t *doc = BCON_NEW("foo", "bar");
        bson_t *opts = BCON_NEW("maxTimeMS", BCON_INT32(1000));
        bool ok = mongoc_collection_insert_one(coll, doc, opts, NULL /* reply */, &error);
        if (!ok)
        {
            printf("error in insert: %s\n", error.message);
            return 1;
        }
        bson_destroy(opts);
        bson_destroy(doc);
    }

    // Set a `maxTimeMS` of 2 seconds on `find`. Server times out if cumulative find and getMores exceed timeout.
    {
        bson_t *filter = bson_new();
        bson_t *opts = BCON_NEW("maxTimeMS", BCON_INT32(2000), "batchSize", BCON_INT32(1));
        mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(coll, filter, opts, NULL /* read_prefs */);

        // Print results.
        const bson_t *doc;
        while (mongoc_cursor_next(cursor, &doc))
        {
            char *doc_str = bson_as_json(doc, NULL);
            printf("got: %s\n", doc_str);
            bson_free(doc_str);
        }

        // Check for error.
        if (mongoc_cursor_error(cursor, &error))
        {
            printf("error in find: %s\n", error.message);
            return 1;
        }

        mongoc_cursor_destroy(cursor);
        bson_destroy(opts);
        bson_destroy(filter);
    }

    mongoc_collection_destroy(coll);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}
