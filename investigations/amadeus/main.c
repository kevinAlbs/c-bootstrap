#include <mongoc/mongoc.h>

typedef struct
{
    int localhost27017;
    int localhost27018;
} counters_t;

static void
command_started(const mongoc_apm_command_started_t *event)
{
    counters_t *counters = mongoc_apm_command_started_get_context(event);
    const mongoc_host_list_t *host = mongoc_apm_command_started_get_host(event);
    if (0 == strcmp(host->host_and_port, "localhost:27017"))
    {
        counters->localhost27017++;
    }
    else if (0 == strcmp(host->host_and_port, "localhost:27018"))
    {
        counters->localhost27018++;
    }
}

static void
server_changed(const mongoc_apm_server_changed_t *event)
{
    const mongoc_server_description_t *prev_sd, *new_sd;

    prev_sd = mongoc_apm_server_changed_get_previous_description(event);
    new_sd = mongoc_apm_server_changed_get_new_description(event);

    MONGOC_DEBUG("server changed: %s %s -> %s",
                 mongoc_apm_server_changed_get_host(event)->host_and_port,
                 mongoc_server_description_type(prev_sd),
                 mongoc_server_description_type(new_sd));
}

int main()
{
    counters_t counters = {0};
    mongoc_client_pool_t *pool;
    char *uristr;

    uristr = getenv("MONGODB_URI");
    if (!uristr)
    {
        uristr = "mongodb://localhost:27017,localhost:27018";
    }
    mongoc_init();

    MONGOC_DEBUG("mongoc_get_version=%s", mongoc_get_version());

    mongoc_uri_t *uri = mongoc_uri_new(uristr);
    pool = mongoc_client_pool_new(uri);

    // Set monitors.
    {
        mongoc_apm_callbacks_t *cbs = mongoc_apm_callbacks_new();
        mongoc_apm_set_command_started_cb(cbs, command_started);
        mongoc_apm_set_server_changed_cb(cbs, server_changed);
        BSON_ASSERT(mongoc_client_pool_set_apm_callbacks(pool, cbs, &counters));
        mongoc_apm_callbacks_destroy(cbs);
    }

    // Repeatedly send ping commands.
    int64_t last_time = bson_get_monotonic_time();
    bson_t *cmd = BCON_NEW("ping", BCON_INT32(1));
    int64_t ONE_SECOND = 1000 * 1000;
    while (true)
    {
        mongoc_client_t *client = mongoc_client_pool_pop(pool);
        bson_error_t error;
        bool ok = mongoc_client_command_simple(client, "db", cmd, NULL /* read prefs */, NULL /* reply */, &error);
        if (!ok)
        {
            MONGOC_ERROR("error: %s", error.message);
            // Do not abort. This may be expected if mongos was shut down.
        }
        mongoc_client_pool_push(pool, client);
        int64_t curr_time = bson_get_monotonic_time();
        if (curr_time - last_time > ONE_SECOND)
        {
            MONGOC_DEBUG("Since last time: %6d commands sent to localhost:27017. %6d commands sent to localhost:27018", counters.localhost27017, counters.localhost27018);
            last_time = curr_time;
            counters = (counters_t){0};
        }
    }
    bson_destroy(cmd);
    mongoc_client_pool_destroy(pool);
    mongoc_cleanup();
}
