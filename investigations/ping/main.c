#include <mongoc/mongoc.h>

void connect_to_mongo ()
{
    bson_error_t error;

    mongoc_init();

    // Parse URI:
    char *uristr = "your-mongodb-atlas-connection-string";    
    mongoc_uri_t *uri = mongoc_uri_new_with_error (uristr, &error);
    if (!uri) {
        fprintf (stderr, "Failed to parse URI: %s", error.message);
        return;
    }
    
    // Create client:
    mongoc_client_t *client = mongoc_client_new_from_uri_with_error(uri, &error);
    if (!client) {
        fprintf(stderr, "Failed to create client: %s\n", error.message);
        return;
    }

    // TODO

    mongoc_client_destroy(client);
    mongoc_uri_destroy (uri);
    mongoc_cleanup();
}


int main () {
    connect_to_mongo();
}
