#include <mongoc/mongoc.h>

int main () {
    mongoc_init ();

    printf ("libmongoc version: %s\n", mongoc_get_version ());
    char *uristr = getenv ("MONGODB_URI") ? getenv ("MONGODB_URI") : "mongodb://localhost:27017";
    mongoc_client_t *client = mongoc_client_new (uristr);
    client = mongoc_client_new (uristr);

    bson_t *ping = BCON_NEW ("ping", BCON_INT32 (1));
    bson_t reply;
    bson_error_t error;
    bool ok = mongoc_client_command_simple (
       client, "admin", ping, NULL, &reply, &error);
    if (!ok) {
        printf ("Ping failed: %s\n", error.message);
    } else {
        printf ("Ping succeeded:\n");
        char *as_json = bson_as_relaxed_extended_json(&reply, NULL);
        printf ("%s\n", as_json);
        bson_free (as_json);
    }
    bson_destroy (&reply);
    bson_destroy (ping);
    
    mongoc_client_destroy (client);
    mongoc_cleanup ();
}
