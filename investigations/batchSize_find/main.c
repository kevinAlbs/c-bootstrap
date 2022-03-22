// Q: Can a different batchSize be set on 'find' and 'getMore' in a cursor from find?
// A: Yes. But in a non-obvious way:
//    - Set the batchSize on 'find' by passing it batchSize in the options to mongoc_collection_find_with_opts. batchSize must be an int64 to avoid the bug of CDRIVER-???
//    - Call mongoc_cursor_next on the cursor.
//    - Call mongoc_cursor_set_batch_size.
#include <mongoc/mongoc.h>
#include "../../util/monitoring.h"

int main () {
    char *uristr;
    mongoc_client_t *client;
    mongoc_apm_callbacks_t *cbs;
    mongoc_collection_t *coll;
    bson_t *filter;
    bson_t *findopts;
    mongoc_cursor_t *cursor;
    bson_error_t error;

    uristr = getenv ("MONGODB_URI");
    if (!uristr) {
        uristr = "mongodb://localhost:27017";
    }
    mongoc_init ();

    MONGOC_DEBUG ("mongoc_get_version=%s", mongoc_get_version ());

    client = mongoc_client_new (uristr);
    cbs = mongoc_apm_callbacks_new ();
    set_command_monitors (cbs, true);
    mongoc_client_set_apm_callbacks (client, cbs, NULL /* context */);
    coll = mongoc_client_get_collection (client, "db", "coll");

    /* Drop and insert into the collection */
    {
        bson_t *to_insert;

        if (!mongoc_collection_drop (coll, &error)) {
            MONGOC_ERROR ("error in mongoc_collection_drop: %s", error.message);
            return EXIT_FAILURE;
        }
        
        to_insert = BCON_NEW ("x", "y");
        if (!mongoc_collection_insert_one (coll, to_insert, NULL /* opts */, NULL /* reply */, &error)) {
            MONGOC_ERROR ("error in mongoc_collection_insert_one: %s", error.message);
            return EXIT_FAILURE;
        }
        if (!mongoc_collection_insert_one (coll, to_insert, NULL /* opts */, NULL /* reply */, &error)) {
            MONGOC_ERROR ("error in mongoc_collection_insert_one: %s", error.message);
            return EXIT_FAILURE;
        }
        bson_destroy (to_insert);
    } 

    filter = bson_new ();
    findopts = BCON_NEW ("batchSize", BCON_INT64(1));
    cursor = mongoc_collection_find_with_opts (coll, filter, findopts, NULL /* read_prefs */);
    
    {
        const bson_t* got;
        char *gotstr;
        MONGOC_DEBUG ("iterating cursor ...");
        while (mongoc_cursor_next (cursor, &got)) {
            gotstr = bson_as_json (got, NULL);
            MONGOC_DEBUG ("got: %s", gotstr);
            bson_free (gotstr);
            mongoc_cursor_set_batch_size (cursor, 2);
        }
        if (mongoc_cursor_error (cursor, &error)) {
            MONGOC_ERROR ("error in mongoc_cursor_next: %s", error.message);
            return EXIT_FAILURE;
        }
    }
    
    mongoc_cursor_destroy (cursor);
    bson_destroy (findopts);
    bson_destroy (filter);
    mongoc_collection_destroy (coll);
    mongoc_apm_callbacks_destroy (cbs);
    mongoc_client_destroy (client);

    mongoc_cleanup ();
}

