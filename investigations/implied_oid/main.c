// Q: Can you insert a "null" ObjectID and have the C driver generate it?
// A: Yes, using bson_oid_init.
#include <mongoc/mongoc.h>

int main () {
    mongoc_client_t *client;
    char *uristr;
    bson_error_t error;

    uristr = getenv ("MONGODB_URI");
    if (!uristr) {
        uristr = "mongodb://localhost:27017";
    }
    mongoc_init ();

    MONGOC_DEBUG ("mongoc_get_version=%s", mongoc_get_version ());
    client = mongoc_client_new (uristr);

    mongoc_collection_t *coll = mongoc_client_get_collection (client, "db", "coll");

    MONGOC_DEBUG ("case 1 ... begin");
    {
        bson_t *to_insert = bson_new_from_json ("{\"second_id\": { \"$oid\": null}}", -1, &error);
        if (!to_insert) {
            // prints "error in bson_new_from_json: Invalid read of null in state IN_BSON_TYPE"
            MONGOC_ERROR ("error in bson_new_from_json: %s", error.message);
        }
    }
    MONGOC_DEBUG ("case 1 ... end");

    MONGOC_DEBUG ("case 2 ... begin");
    {
        bson_t *to_insert = bson_new ();
        bson_oid_t oid;
        bson_oid_init (&oid, NULL /* context */);
        bson_append_oid (to_insert, "second_id", -1, &oid);
        if (!mongoc_collection_insert_one (coll, to_insert, NULL /* opts */, NULL /* reply */, &error)) {
            MONGOC_ERROR ("error in mongoc_collection_insert_one: %s", error.message);
            return EXIT_FAILURE;
        }
        MONGOC_DEBUG ("insert OK");
        bson_destroy (to_insert);
    }
    MONGOC_DEBUG ("case 2 ... end");

    mongoc_collection_destroy (coll);

    mongoc_client_destroy (client);
    mongoc_cleanup ();
}
