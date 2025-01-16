// Repeatedly runs `find` commands with a client pool to "db.coll".
// Configure with environment variables:
//  THREAD_COUNT to set a thread count. Defaults to 10.
//  MONGODB_URI to set a URI. Defaults to mongodb://localhost:27017

#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
    int tid;
    int thread_count;
    int host_count;
    mongoc_client_pool_t *pool;
} thread_args_t;

#define DB "db"
#define COLL "coll"

/* thread_find repeatedly runs find */
void *thread_find(void *arg)
{
    mongoc_client_pool_t *pool;
    bson_t filter = BSON_INITIALIZER;
    int64_t ops_trial = 0;
    int64_t ops_total = 0;
    bson_error_t error;
    thread_args_t *args = (thread_args_t *)arg;
    int64_t start_time;
    int trial_count = 0;

    pool = args->pool;

    mongoc_client_t *client = mongoc_client_pool_pop(pool);

    // First warm up connections.
    {
        bson_t ping = BSON_INITIALIZER;
        BSON_APPEND_INT32(&ping, "ping", 1);

        // Send a ping to each host.
        for (int server_id = 1; server_id <= args->host_count; server_id++)
        {
            bool ok = mongoc_client_command_simple_with_server_id(client, "db", &ping, NULL, server_id, NULL, &error);
            if (!ok)
            {
                MONGOC_ERROR("Failed to ping host with id: %d: %s", server_id, error.message);
                abort();
            }
        }
        bson_destroy(&ping);
    }

    start_time = bson_get_monotonic_time();
    while (true)
    {
        mongoc_collection_t *coll;
        mongoc_cursor_t *cursor;
        const bson_t *doc;

        coll = mongoc_client_get_collection(client, DB, COLL);
        cursor = mongoc_collection_find_with_opts(coll, &filter, NULL /* opts */, NULL /* read_prefs */);
        if (mongoc_cursor_error(cursor, &error))
        {
            MONGOC_ERROR("[tid=%d] find returned error: %s", args->tid, error.message);
            abort();
        }
        /* iterate once */
        if (mongoc_cursor_next(cursor, &doc))
        {
            MONGOC_ERROR("unexpected document returned. %s.%s must be an empty collection", DB, COLL);
            abort();
        }
        if (mongoc_cursor_error(cursor, &error))
        {
            MONGOC_ERROR("[tid=%d] next returned error: %s", args->tid, error.message);
            abort();
        }
        ops_trial += 1;
        if (ops_trial >= 5000)
        {
            // Check time, estimate ops/sec.
            ops_total += ops_trial;
            int64_t current_time = bson_get_monotonic_time();
            double ops_per_sec = (double)ops_total / (((double)(current_time - start_time)) / (1000 * 1000));
            ops_trial = 0;
            trial_count++;
            if (args->tid == 0)
            {
                MONGOC_INFO("After %d trials, estimated ops/s for all threads: %.02f", trial_count, ops_per_sec * args->thread_count);
            }
        }

        mongoc_cursor_destroy(cursor);
        mongoc_collection_destroy(coll);
    }

    bson_destroy(&filter);
    mongoc_client_pool_push(pool, client);

    return NULL;
}

int main(int argc, char *argv[])
{
    mongoc_uri_t *uri = NULL;
    mongoc_client_pool_t *pool = NULL;
    pthread_t *threads = NULL;
    thread_args_t *thread_args = NULL;
    int thread_count = 0;

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
    pool = mongoc_client_pool_new(uri);

    const char *thread_count_str = getenv("THREAD_COUNT");
    thread_count = thread_count_str ? atoi(thread_count_str) : 10;
    if (thread_count_str)
    {
        thread_count = atoi(thread_count_str);
        if (!thread_count)
        {
            MONGOC_ERROR("invalid thread count: %s", thread_count_str);
            abort();
        }
    }
    else
    {
        printf("Using default thread count of 10. Use THREAD_COUNT environment variable to configure.\n");
        thread_count = 10;
    }

    threads = bson_malloc0(sizeof(pthread_t) * thread_count);
    thread_args = bson_malloc0(sizeof(thread_args_t) * thread_count);

    // Drop collection.
    {
        mongoc_client_t *client = mongoc_client_pool_pop(pool);
        mongoc_collection_t *coll = mongoc_client_get_collection(client, DB, COLL);
        mongoc_collection_drop(coll, NULL);
        mongoc_collection_destroy(coll);
        mongoc_client_pool_push(pool, client);
    }

    // Count number of hosts.
    int host_count = 0;
    {
        for (const mongoc_host_list_t *hosts = mongoc_uri_get_hosts(uri); hosts != NULL; hosts = hosts->next)
        {
            host_count++;
        }
    }

    for (int i = 0; i < thread_count; i++)
    {
        thread_args[i].pool = pool;
        thread_args[i].tid = i;
        thread_args[i].thread_count = thread_count;
        thread_args[i].host_count = host_count;
        pthread_create(threads + i, NULL /* pthread_attr_t */, thread_find, thread_args + i);
    }

    MONGOC_INFO("Using thread count: %d", thread_count);
    MONGOC_INFO("Using C driver version: %s", mongoc_get_version());

    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }

    bson_free(threads);
    mongoc_client_pool_destroy(pool);
    mongoc_uri_destroy(uri);
    mongoc_cleanup();
}
