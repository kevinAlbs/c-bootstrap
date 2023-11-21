#include <mongoc/mongoc.h>

int main () {
    mongoc_client_t *client;
    char *uristr;

    uristr = getenv ("MONGODB_URI");
    if (!uristr) {
        uristr = "mongodb://localhost:27017";
    }
    mongoc_init ();

    MONGOC_DEBUG ("mongoc_get_version=%s", mongoc_get_version ());
    client = mongoc_client_new (uristr);
    mongoc_collection_t *coll = mongoc_client_get_collection (client, "db", "coll");
    bson_t *cmd = BCON_NEW ("ping", BCON_INT32(1));
    bson_error_t error;

    // The bson_t struct `*reply_on_heap` is heap allocated.
    // `bson_get_data(reply_on_heap)` may be containined within the `bson_t` struct or heap allocated separately depending on the size of the data.
    bson_t* reply_on_heap = bson_malloc (sizeof (bson_t));
    bool ok = mongoc_collection_command_simple (coll, cmd,
                                    NULL /* read_prefs */,
                                    reply_on_heap,
                                    &error);
    // OK. `reply_on_heap` is initialized. No leak.
    bson_destroy (reply_on_heap); // Free data referred to by `bson_t`.
    bson_free (reply_on_heap); // Free `bson_t` struct.
    
    bson_destroy (cmd);
    mongoc_collection_destroy (coll);
    mongoc_client_destroy (client);
    mongoc_cleanup ();
}
