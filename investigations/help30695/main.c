#include <mongoc/mongoc.h>

int main()
{
    mongoc_client_t *client;
    char *uristr;
    bson_t doc;
    mongoc_collection_t *coll;
    bson_error_t error;
    bool ret;

    uristr = getenv("MONGODB_URI");
    if (!uristr)
    {
        uristr = "mongodb://localhost:27017";
    }
    mongoc_init();

    client = mongoc_client_new(uristr);
    bson_init(&doc);

    coll = mongoc_client_get_collection(client, "db", "coll");
    ret = mongoc_collection_insert_one(coll, &doc, NULL /* opts */, NULL /* reply */, &error);
    if (!ret)
    {
        MONGOC_ERROR("mongoc_collection_insert_one error: %s", error.message);
        return EXIT_FAILURE;
    }

    bson_destroy(&doc);
    mongoc_client_destroy(client);

    mongoc_cleanup();
}
