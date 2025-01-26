// Repeatedly runs `find` commands with a client pool to "db.coll".
// Configure with environment variables:
//  MONGODB_URI to set a URI. Defaults to mongodb://localhost:27017

#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
    int n_heartbeat_failed;
    int n_server_changed;
    int n_server_closed;
    int n_command_failed;
} counts_t;

void heartbeat_failed(const mongoc_apm_server_heartbeat_failed_t *event)
{
    counts_t *counts = mongoc_apm_server_heartbeat_failed_get_context(event);
    counts->n_heartbeat_failed++;
}

void server_changed(const mongoc_apm_server_changed_t *event)
{
    counts_t *counts = mongoc_apm_server_changed_get_context(event);
    counts->n_server_changed++;
}

void server_closed(const mongoc_apm_server_closed_t *event)
{
    counts_t *counts = mongoc_apm_server_closed_get_context(event);
    counts->n_server_closed++;
}

void command_failed(const mongoc_apm_command_failed_t *event)
{
    counts_t *counts = mongoc_apm_command_failed_get_context(event);
    counts->n_command_failed++;
}

int cmpi64(const void *a, const void *b)
{
    int64_t int_a = *(int64_t *)a;
    int64_t int_b = *(int64_t *)b;

    if (int_a < int_b)
        return -1;
    if (int_a > int_b)
        return 1;
    return 0;
}
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
    uri = mongoc_uri_new(uri_str);
    const mongoc_host_list_t *hosts = mongoc_uri_get_hosts(uri);
    size_t nhosts = 0;
    while (hosts != NULL)
    {
        hosts = hosts->next;
        nhosts++;
    }
    printf("Using URI with %zu hosts\n", nhosts);

    if (!uri)
    {
        MONGOC_ERROR("invalid URI %s", uri_str);
        abort();
    }

    // Set a short heartbeat to produce more topology updates:
    mongoc_uri_set_option_as_int32(uri, MONGOC_URI_HEARTBEATFREQUENCYMS, 500);

    mongoc_client_pool_t *pool = mongoc_client_pool_new(uri);

    counts_t counts = {0};

    {
        mongoc_apm_callbacks_t *cbs = mongoc_apm_callbacks_new();
        cbs = mongoc_apm_callbacks_new();
        mongoc_apm_set_server_heartbeat_failed_cb(cbs, heartbeat_failed);
        mongoc_apm_set_server_changed_cb(cbs, server_changed);
        mongoc_apm_set_command_failed_cb(cbs, command_failed);
        mongoc_apm_set_server_closed_cb(cbs, server_closed);
        mongoc_client_pool_set_apm_callbacks(pool, cbs, &counts);
        mongoc_apm_callbacks_destroy(cbs);
    }

    // Drop collection.
    {
        mongoc_client_t *client = mongoc_client_pool_pop(pool);
        mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");
        mongoc_collection_drop(coll, NULL);
        mongoc_collection_destroy(coll);
        mongoc_client_pool_push(pool, client);
    }

    size_t n_trials = 5;
    int64_t results[n_trials];
    size_t num_finds = 1000;
    // printf("Call count of mongoc_server_description_new_copy: %zu\n", get_mongoc_server_description_new_copy_callcount());

    // Do many finds.
    for (size_t i = 0; i < n_trials; i++)
    {
        printf(".");
        fflush(stdout);
        bson_t filter = BSON_INITIALIZER;
        int64_t opcount = 0;
        bson_error_t error;
        int64_t start_time;

        mongoc_client_t *client = mongoc_client_pool_pop(pool);

        // Warm up connections.
        {
            bson_t *ping = BCON_NEW("ping", BCON_INT32(1));
            for (size_t serverid = 1; serverid < nhosts + 1; serverid++)
            {
                if (!mongoc_client_command_simple_with_server_id(client, "db", ping, NULL, serverid, NULL, &error))
                {
                    MONGOC_ERROR("failed to ping server: %s\n", error.message);
                    abort();
                }
            }
            bson_destroy(ping);
        }

        mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");

        start_time = bson_get_monotonic_time();
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
                int64_t current_time = bson_get_monotonic_time();
                results[i] = current_time - start_time;
                break;
            }
        }

        mongoc_collection_destroy(coll);
        bson_destroy(&filter);
        mongoc_client_pool_push(pool, client);
    }

    qsort(results, n_trials, sizeof(int64_t), cmpi64);
    int64_t median_time = results[n_trials / 2];

    // Check time, estimate ops/sec.
    double total_time = (double)median_time / (1000.0 * 1000);
    double ops_per_sec = (double)num_finds / total_time;
    printf("\n");
    printf("median time: %g seconds\n", total_time);
    printf("ops/sec    : %g\n", ops_per_sec);
    // printf("Call count of mongoc_server_description_new_copy: %zu\n", get_mongoc_server_description_new_copy_callcount());
    printf("server change events: %d\n", counts.n_server_changed);
    printf("heartbeat failed events: %d\n", counts.n_heartbeat_failed);
    printf("command failed events: %d\n", counts.n_command_failed);
    printf("server closed events: %d\n", counts.n_server_closed);

    mongoc_client_pool_destroy(pool);
    mongoc_uri_destroy(uri);
    mongoc_cleanup();
}
