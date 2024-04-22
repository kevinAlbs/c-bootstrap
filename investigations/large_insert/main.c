#include <mongoc/mongoc.h>

int main()
{
    mongoc_client_t *client;
    char *uristr;

    uristr = getenv("MONGODB_URI");
    if (!uristr)
    {
        uristr = "mongodb://localhost:27017";
    }
    mongoc_init();

    MONGOC_DEBUG("mongoc_get_version=%s", mongoc_get_version());
    client = mongoc_client_new(uristr);

    // Make a sequence of documents totalling a size > INT_MAX bytes.
#define NUM_DOCS 256
    bson_t *docs[NUM_DOCS];
    const size_t maxBsonObjSize = 16 * 1024 * 1024;
    const size_t overhead = 64;
    size_t large_size = maxBsonObjSize - overhead;  // Subtract overhead to keep each individual document under `maxBsonObjSize`
    char *large_str = bson_malloc0(large_size + 1); // Add one for trailing NULL.
    memset(large_str, 'a', large_size);

    for (size_t i = 0; i < NUM_DOCS; i++)
    {
        docs[i] = bson_new();
        BSON_APPEND_UTF8(docs[i], "large_str", large_str);
    }

    // Check that total size of all documents is greater than INT_MAX.
    size_t total = 0;
    for (size_t i = 0; i < NUM_DOCS; i++)
    {
        total += docs[i]->len;
    }
    BSON_ASSERT(total >= INT_MAX);

    bson_error_t error;
    mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");
    printf("Inserting ... (this may take a while)\n");
    bool ok = mongoc_collection_insert_many(coll, (const bson_t **)docs, NUM_DOCS, NULL, NULL, &error);
    if (!ok)
    {
        printf("got error: %s\n", error.message);
    }
    printf("Inserting ... done\n");

    bson_free(large_str);
    for (size_t i = 0; i < NUM_DOCS; i++)
    {
        bson_destroy(docs[i]);
    }
    mongoc_collection_destroy(coll);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}
