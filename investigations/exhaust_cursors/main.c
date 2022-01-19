#include <mongoc/mongoc.h>

#include "../../util/monitoring.h"

int main () {
    const char *uristr;
    mongoc_client_t *client;
    bson_error_t error;
    mongoc_apm_callbacks_t *callbacks;

    mongoc_init ();

    uristr = getenv ("MONGODB_URI");
    if (!uristr) {
        uristr = "mongodb://localhost:27017";
    }

    callbacks = mongoc_apm_callbacks_new ();
    set_command_monitors (callbacks, false /* started_only */);
    client = mongoc_client_new (uristr);

    {
        mongoc_collection_t *coll;
        int i;

        /* drop db.coll and insert 8 documents. */
        coll = mongoc_client_get_collection (client, "db", "coll");
        mongoc_collection_drop (coll, &error); /* ignore error from 'ns not found'. */
        for (i = 0; i < 8; i++) {
            bson_t * to_insert = BCON_NEW ("_id", BCON_INT32(i));
            if (!mongoc_collection_insert_one (coll, to_insert, NULL /* opts */, NULL /* reply */, &error)) {
                MONGOC_ERROR ("error on insert: %s", error.message);
                return EXIT_FAILURE;
            }
        }

        if (!getenv ("QUIET")) {
            mongoc_client_set_apm_callbacks (client, callbacks, NULL /* context */);    
        }

        {
            mongoc_cursor_t *cursor;
            const bson_t *doc;
            bson_t *opts = NULL;
            bson_t *filter = bson_new ();

            /* Create a cursor with batchSize=5, and exhaust=true. */
            opts = BCON_NEW("batchSize", BCON_INT32(5), "exhaust", BCON_BOOL(true));
            cursor = mongoc_collection_find_with_opts(coll, filter, opts, NULL /* read prefs */);
            while (mongoc_cursor_next (cursor, &doc)) {
                char *str;

                str = bson_as_json (doc, NULL);
                MONGOC_DEBUG ("mongoc_cursor_next got: %s", str);
                bson_free (str);
            }

            if (mongoc_cursor_error (cursor, &error)) {
                MONGOC_ERROR ("error on mongoc_cursor_next: %s", error.message);
                return EXIT_FAILURE;
            }

            bson_destroy (opts);
            mongoc_cursor_destroy(cursor);
            bson_destroy (filter);
                
        }
        mongoc_collection_destroy (coll);
    }

    mongoc_apm_callbacks_destroy (callbacks);
    mongoc_client_destroy (client);

    mongoc_cleanup ();
}
