#include <mongoc/mongoc.h>

int main()
{
    mongoc_uri_t *uri;
    mongoc_client_pool_t *pool;
    mongoc_client_t *client;
    mongoc_collection_t *coll;
    char *uristr;
    int i;
    bson_error_t error;

    mongoc_init();

    uristr = getenv("MONGODB_URI");
    if (!uristr)
    {
        uristr = "mongodb://localhost:27017";
    }

    uri = mongoc_uri_new (uristr);
    pool = mongoc_client_pool_new (uri);
    client = mongoc_client_pool_pop(pool);
    coll = mongoc_client_get_collection(client, "db", "coll");

    /* Drop and insert 3 documents. */
    mongoc_collection_drop(coll, NULL);
    for (i = 0; i < 3; i++)
    {
        bson_t *to_insert = BCON_NEW("_id", BCON_INT32(i));
        bool res = mongoc_collection_insert_one(coll, to_insert, NULL /* opts */, NULL /* reply */, &error);
        if (!res)
        {
            MONGOC_ERROR("error in insert_one: %s", error.message);
            return EXIT_FAILURE;
        }
        bson_destroy(to_insert);
    }

    for (i = 0; i < 10; i++)
    {
        mongoc_cursor_t *cursor;
        bson_t *selector = bson_new();
        const bson_t *doc;

        cursor = mongoc_collection_find(
            coll,
            MONGOC_QUERY_EXHAUST,
            0 /* skip */,
            1 /* limit */,
            0 /* batch_size */,
            selector,
            NULL /* fields */,
            NULL /* read prefs */);

        while (mongoc_cursor_next(cursor, &doc))
        {
            char *docstr = bson_as_json(doc, NULL);
            MONGOC_DEBUG("cursor got: %s", docstr);
            bson_free(docstr);
        }

        if (mongoc_cursor_error (cursor, &error)) {
            MONGOC_ERROR ("cursor error: %s", error.message);
            return EXIT_FAILURE;
        }

        bson_destroy(selector);
        mongoc_cursor_destroy(cursor);
    }

    mongoc_collection_destroy(coll);
    mongoc_client_pool_push (pool, client);
    mongoc_client_pool_destroy (pool);
    mongoc_uri_destroy (uri);

    mongoc_cleanup();
}
