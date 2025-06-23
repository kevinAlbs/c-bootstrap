#include <mongoc/mongoc.h>

static void
server_opening(const mongoc_apm_server_opening_t *event)
{
    printf("server opening: %s\n", mongoc_apm_server_opening_get_host(event)->host_and_port);
}

int main()
{
    char *uristr = getenv("MONGODB_URI");
    if (!uristr)
    {
        printf("Set required environment variable: MONGODB_URI\n");
        return 1;
    }

    mongoc_init();

    printf("mongoc_get_version=%s\n", mongoc_get_version());

    mongoc_client_t *client = mongoc_client_new(uristr);
    if (!client)
    {
        return 1;
    }

    // Set server monitoring events.
    {
        mongoc_apm_callbacks_t *cbs = mongoc_apm_callbacks_new();
        mongoc_apm_set_server_opening_cb(cbs, server_opening);
        mongoc_client_set_apm_callbacks(client, cbs, NULL);
    }

    // Send a ping command.
    {
        bson_error_t error;

        bson_t *ping = BCON_NEW("ping", BCON_INT32(1));
        bool ok = mongoc_client_command_simple(client, "db", ping, NULL, NULL, &error);
        if (!ok)
        {
            printf("Error on ping: %s\n", error.message);
            return 1;
        }

        printf("Ping succeeded\n");
        bson_destroy(ping);
    }

    mongoc_client_destroy(client);
    mongoc_cleanup();
}
