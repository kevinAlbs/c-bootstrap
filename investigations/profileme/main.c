#include <mongoc/mongoc.h>

int main()
{
    mongoc_init();

    printf("Using C driver version: %s\n", mongoc_get_version());
    mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017");

    int64_t start = bson_get_monotonic_time();
    bson_t *ping = BCON_NEW("ping", BCON_INT32(1));
    bson_error_t error;
    printf("Sending 'ping' commands on repeat ... begin\n");
    while (true)
    {
        if (!mongoc_client_command_simple(client, "admin", ping, NULL, NULL, &error))
        {
            printf("Error sending 'ping': %s", error.message);
            abort();
        }
        int64_t now = bson_get_monotonic_time();
        int64_t diff = now - start;
        if (diff >= 30 * 1000 * 1000)
        {
            break;
        }
    }
    printf("Sending 'ping' commands on repeat ... end\n");

    bson_destroy(ping);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}
