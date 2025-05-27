// Usage: post "comment"

#include <mongoc/mongoc.h>

int main(int argc, char *argv[])
{
    mongoc_init();
    mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017");
    mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "comments");
    bson_error_t error;

    if (argc != 2) {
        fprintf(stderr, "Usage: post 'comment'\n");
        return 1;
    }

    bson_t *doc = BCON_NEW("comment", BCON_UTF8(argv[1]));

    // Ensure document has valid UTF-8:
    if (!bson_validate_with_error(doc, BSON_VALIDATE_UTF8, &error)) {
        fprintf(stderr, "Failed to validate: %s\n", error.message);
        return 1;
    }

    // Insert:
    if (!mongoc_collection_insert_one(coll, doc, NULL, NULL, &error)) {
        fprintf(stderr, "Failed to insert: %s\n", error.message);
        return 1;
    }

    printf ("Inserted\n");
    bson_destroy(doc);
    mongoc_collection_destroy(coll);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}
