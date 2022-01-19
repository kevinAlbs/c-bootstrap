#include <mongoc/mongoc.h>
#include "../../util/monitoring.h"

int main () {
    mongoc_init ();

    bson_error_t error;
    mongoc_apm_callbacks_t *cb;
    bson_t* doc = BCON_NEW ("key", "a", "key", "b");
    mongoc_client_t *client = mongoc_client_new (NULL);

    cb = mongoc_apm_callbacks_new ();
    set_command_monitors (cb, true);
    mongoc_client_set_apm_callbacks (client, cb, NULL /* context */);
    mongoc_collection_t *coll = mongoc_client_get_collection (client, "db", "coll");
    if (!mongoc_collection_insert_one (coll, doc, NULL /* opts */, NULL /* reply */, &error)) {
        MONGOC_ERROR ("insert_one error: %s", error.message);
    }

    mongoc_collection_destroy (coll);
    mongoc_apm_callbacks_destroy (cb);
    mongoc_client_destroy (client);
    bson_destroy (doc);

    mongoc_cleanup ();
}
