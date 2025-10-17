// Q: Can `mongoc_server_description_round_trip_time` return -1 to users? A: Yes.
//
// Sample output:
//
//  mongoc_get_version=2.0.2
//  server changed: localhost:27777 Unknown [RTT=-1] -> RSPrimary [RTT=2]
//
#include <mongoc/mongoc.h>
#include <stdio.h>

static BSON_INLINE void
server_changed(const mongoc_apm_server_changed_t *event)
{
    const mongoc_server_description_t *prev_sd, *new_sd;

    prev_sd = mongoc_apm_server_changed_get_previous_description(event);
    new_sd = mongoc_apm_server_changed_get_new_description(event);

    printf("server changed: %s %s [RTT=%" PRId64 "] -> %s [RTT=%" PRId64 "]\n",
           mongoc_apm_server_changed_get_host(event)->host_and_port,
           mongoc_server_description_type(prev_sd),
           mongoc_server_description_round_trip_time(prev_sd),
           mongoc_server_description_type(new_sd),
           mongoc_server_description_round_trip_time(new_sd));
}

static BSON_INLINE void monitor(mongoc_client_t *client)
{
    mongoc_apm_callbacks_t *cbs = mongoc_apm_callbacks_new();
    mongoc_apm_set_server_changed_cb(cbs, server_changed);
    mongoc_client_set_apm_callbacks(client, cbs, NULL);
    mongoc_apm_callbacks_destroy(cbs);
}

int main()
{
    mongoc_client_t *client;
    char *uristr;

    uristr = getenv("MONGODB_URI") ? getenv("MONGODB_URI") : "mongodb://localhost:27017";

    mongoc_init();

    printf("mongoc_get_version=%s\n", mongoc_get_version());
    client = mongoc_client_new(uristr);
    monitor(client);

    bson_t *ping = BCON_NEW("ping", BCON_INT32(1));
    bson_error_t error;
    bool ok =
        mongoc_client_command_simple(client, "admin", ping, NULL, NULL, &error);
    if (!ok)
    {
        MONGOC_DEBUG("ping failed: %s", error.message);
    }

    mongoc_client_destroy(client);
    mongoc_cleanup();
}
