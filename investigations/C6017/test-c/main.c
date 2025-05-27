#include <mongoc/mongoc.h>
#include "../../../util/assertions.h"

int main()
{
    printf("Testing libmongoc: %s\n", mongoc_get_version());
    mongoc_init();
    mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017");
    mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");
    mongoc_collection_drop(coll, NULL);
    bson_error_t error;

    {
        bson_t to_insert = BSON_INITIALIZER;
        BSON_APPEND_UTF8 (&to_insert, "bad", "\xFF");
        bool ok = mongoc_collection_insert_one (coll, &to_insert, NULL, NULL, &error); // No error
        ASSERT (ok); // No error.
        char *as_json = bson_as_canonical_extended_json (&to_insert, NULL);
        ASSERT (!as_json); // Error.
    }


    printf ("Insert invalid UTF-8 string ... ");
    {
        bson_t to_insert = BSON_INITIALIZER;
        ASSERT(bson_append_utf8(&to_insert, "bad", 3, "\xFF", 1));
        bool ok = mongoc_collection_insert_one(coll, &to_insert, NULL, NULL, &error);
        // Before PR  : ok
        // After PR   : corrupt BSON
        if (ok) {
            printf ("ok\n");
        } else {
            printf ("failed with error: %s\n", error.message);
        }
        bson_destroy(&to_insert);
    }

    printf ("Insert invalid UTF-8 key ... ");
    {
        bson_t to_insert = BSON_INITIALIZER;
        ASSERT(bson_append_utf8(&to_insert, "\xFF", 1, "bad", 3));
        bool ok = mongoc_collection_insert_one(coll, &to_insert, NULL, NULL, &error);
        // Before PR  : corrupt BSON
        // After PR   : corrupt BSON
        if (ok) {
            printf ("ok\n");
        } else {
            printf ("failed with error: %s\n", error.message);
        }
        bson_destroy(&to_insert);
    }

    mongoc_collection_destroy(coll);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}
