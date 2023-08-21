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

    /* TODO */
    
    mongoc_client_destroy (client);
    mongoc_cleanup ();
}
