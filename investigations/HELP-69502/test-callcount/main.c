// Repeatedly runs `find` commands with a client pool to "db.coll".
// Configure with environment variables:
//  MONGODB_URI to set a URI. Defaults to mongodb://localhost:27017

#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int main(int argc, char *argv[])
{
    mongoc_uri_t *uri = NULL;

    mongoc_init();

    const char *uri_str = getenv("MONGODB_URI");
    if (!uri_str)
    {
        printf("Using default URI. Use MONGODB_URI environment variable to configure.\n");
        uri_str = "mongodb://localhost:27017";
    }
    printf("Using URI: %s\n", uri_str);
    uri = mongoc_uri_new(uri_str);

    if (!uri)
    {
        MONGOC_ERROR("invalid URI %s", uri_str);
        abort();
    }

    // Set a short heartbeat to produce more topology updates:
    mongoc_uri_set_option_as_int32(uri, MONGOC_URI_HEARTBEATFREQUENCYMS, 500);

    mongoc_client_pool_t *pool = mongoc_client_pool_new(uri);

    // Drop collection.
    {
        mongoc_client_t *client = mongoc_client_pool_pop(pool);
        mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");
        mongoc_collection_drop(coll, NULL);
        mongoc_collection_destroy(coll);
        mongoc_client_pool_push(pool, client);
    }

    size_t num_finds = 5000;
    printf("Call count of mongoc_server_description_new_copy: %zu\n", get_mongoc_server_description_new_copy_callcount());
    printf("Running %zu finds...\n", num_finds);

    // Do many finds.
    {
        bson_t filter = BSON_INITIALIZER;
        int64_t opcount = 0;
        bson_error_t error;
        int64_t start_time;

        mongoc_client_t *client = mongoc_client_pool_pop(pool);
        mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");

        start_time = bson_get_monotonic_time();
        mongoc_server_description_new_copy(NULL);
        while (true)
        {
            mongoc_cursor_t *cursor;
            const bson_t *doc;

            cursor = mongoc_collection_find_with_opts(coll, &filter, NULL /* opts */, NULL /* read_prefs */);
            if (mongoc_cursor_next(cursor, &doc))
            {
                MONGOC_ERROR("unexpected document returned.");
                abort();
            }
            if (mongoc_cursor_error(cursor, &error))
            {
                MONGOC_ERROR("next returned error: %s", error.message);
                abort();
            }
            opcount += 1;
            mongoc_cursor_destroy(cursor);

            if (opcount >= num_finds)
            {
                // Check time, estimate ops/sec.
                int64_t current_time = bson_get_monotonic_time();
                double ops_per_sec = (double)opcount / (((double)(current_time - start_time)) / (1000 * 1000));
                double total_time = (double)(current_time - start_time) / (1000 * 1000);

                printf("ops/sec   : %g\n", ops_per_sec);
                printf("total time: %g seconds\n", total_time);
                break;
            }
        }

        mongoc_collection_destroy(coll);
        bson_destroy(&filter);
        mongoc_client_pool_push(pool, client);
    }

    printf("Running %zu finds... done\n", num_finds);
    printf("Call count of mongoc_server_description_new_copy: %zu\n", get_mongoc_server_description_new_copy_callcount());

    mongoc_client_pool_destroy(pool);
    mongoc_uri_destroy(uri);
    mongoc_cleanup();
}
