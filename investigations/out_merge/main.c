#include <mongoc/mongoc.h>
#include "../../util/monitoring.h"

int main () {
    mongoc_client_t *client;
    mongoc_collection_t *coll;
    char *uri;
    bson_t *pipeline;
    mongoc_read_prefs_t *read_prefs;
    mongoc_cursor_t *cursor;
    const bson_t *placeholder;
    bool ret;
    bson_error_t error;
    mongoc_apm_callbacks_t *cbs;
    
    mongoc_init ();
    uri = getenv ("MONGODB_URI");

    if (uri == NULL) {
        uri = strdup ("mongodb://localhost:27017");
    }

    client = mongoc_client_new (uri);
    cbs = mongoc_apm_callbacks_new ();
    set_command_monitors (cbs, true /* started_only */);
    mongoc_client_set_apm_callbacks (client, cbs, NULL /* context */);
    coll = mongoc_client_get_collection (client, "db", "coll");
    pipeline = BCON_NEW ("pipeline", "[",
        "{", "$match", "{", "}", "}",
        "{", "$out", "{", "db", "db", "coll", "coll2", "}", "}",
    "]");

    read_prefs = mongoc_read_prefs_new (MONGOC_READ_SECONDARY);
    cursor = mongoc_collection_aggregate (coll, MONGOC_QUERY_NONE, pipeline, NULL /* opts */, read_prefs);
    ret = mongoc_cursor_next (cursor, &placeholder);
    /* Expect 'ret' to be false. There are no returned documents. */
    if (ret) {
        MONGOC_ERROR ("Expected mongoc_cursor_next to return false, got true");
        return EXIT_FAILURE;
    }

    if (mongoc_cursor_error (cursor, &error)) {
        MONGOC_ERROR ("Error in cursor: %s", error.message);
        return EXIT_FAILURE;
    }

    mongoc_cursor_destroy (cursor);
    bson_destroy (pipeline);
    mongoc_collection_destroy (coll);
    mongoc_apm_callbacks_destroy (cbs);
    mongoc_client_destroy (client);

    mongoc_cleanup ();
    free (uri);
}
