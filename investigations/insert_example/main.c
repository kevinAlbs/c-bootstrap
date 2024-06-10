#include <mongoc/mongoc.h>

int main()
{
    mongoc_init();

    mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017");
    mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");

    MONGOC_EXPORT(bool)
    mongoc_collection_insert_one(
        mongoc_collection_t * collection, const bson_t *document, const bson_t *opts, bson_t *reply, bson_error_t *error);

    bson_t *to_insert = BCON_NEW("foo", "bar");
    bson_t reply;
    bson_error_t error;
    bool ok = mongoc_collection_insert_one(coll, to_insert, NULL, &reply, &error);
    if (!ok)
    {
        printf("error inserting: %s\n", error.message);
        abort();
    }

    char *reply_json = bson_as_json(&reply, NULL);
    printf("reply_json=%s\n", reply_json);
    bson_free(reply_json);

    mongoc_collection_destroy(coll);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}
