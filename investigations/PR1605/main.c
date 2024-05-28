// Example printing stats of commands sent in cursor operations.

#include <mongoc/mongoc.h>

typedef struct
{
    int64_t find_count;
    int64_t find_duration_sum;
    int64_t getMore_count;
    int64_t getMore_duration_sum;
} cursor_stats_t;

static void
command_succeeded(const mongoc_apm_command_succeeded_t *event)
{
    cursor_stats_t *stats = (cursor_stats_t *)mongoc_apm_command_succeeded_get_context(event);
    const char *cmd_name = mongoc_apm_command_succeeded_get_command_name(event);
    int64_t duration = mongoc_apm_command_succeeded_get_duration(event);
    if (0 == strcmp("find", cmd_name))
    {
        stats->find_count++;
        stats->find_duration_sum += duration;
    }
    else if (0 == strcmp("getMore", cmd_name))
    {
        stats->getMore_count++;
        stats->getMore_duration_sum += duration;
    }
}

int main()
{
    bool ok;
    bson_error_t error;
    cursor_stats_t stats = {0};
    mongoc_client_t *client;

    mongoc_init();

    printf("Using C driver version: %s\n", mongoc_get_version());
    client = mongoc_client_new("mongodb://localhost:27017");

    // Set command monitoring events to get command durations.
    {
        mongoc_apm_callbacks_t *cbs = mongoc_apm_callbacks_new();
        mongoc_apm_set_command_succeeded_cb(cbs, command_succeeded);
        BSON_ASSERT(mongoc_client_set_apm_callbacks(client, cbs, &stats));
        mongoc_apm_callbacks_destroy(cbs);
    }

    mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");

    // Drop collection to clear prior data.
    mongoc_collection_drop(coll, NULL); // Ignore error.

    // Insert three documents.
    {
        bson_t *docs[] = {
            BCON_NEW("_id", BCON_INT32(1)),
            BCON_NEW("_id", BCON_INT32(2)),
            BCON_NEW("_id", BCON_INT32(3))};

        if (!mongoc_collection_insert_many(coll, (const bson_t **)docs, 3, NULL, NULL, &error))
        {
            printf("failed to insert: %s\n", error.message);
            abort();
        }

        bson_destroy(docs[0]);
        bson_destroy(docs[1]);
        bson_destroy(docs[2]);
    }

    // Create a cursor with batch size 1.
    mongoc_cursor_t *cursor;
    {
        bson_t *filter = bson_new();
        bson_t *opts = BCON_NEW("batchSize", BCON_INT32(2));
        cursor = mongoc_collection_find_with_opts(coll, filter, opts, NULL);
        bson_destroy(opts);
        bson_destroy(filter);
    }

    // Expect no commands are sent.
    {
        BSON_ASSERT(stats.find_count == 0);
        BSON_ASSERT(stats.getMore_count == 0);
    }

    // Iterate to completion.
    const bson_t *doc;
    while (mongoc_cursor_next(cursor, &doc))
        ;

    if (mongoc_cursor_error(cursor, &error))
    {
        printf("Got error on cursor: %s\n", error.message);
        abort();
    }

    // Print statistics.
    printf("'find' sent           : %" PRId64 "\n", stats.find_count);
    printf("'find' avg duration   : %.02f\n", stats.find_duration_sum / (double)stats.find_count);
    printf("'getMore' sent        : %" PRId64 "\n", stats.getMore_count);
    printf("'getMore' avg duration: %.02f\n", stats.getMore_duration_sum / (double)stats.getMore_count);

    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(coll);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}
