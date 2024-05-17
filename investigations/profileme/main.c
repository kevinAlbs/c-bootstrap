#include <mongoc/mongoc.h>

int main()
{
    mongoc_client_t *client;
    char *uristr;

    uristr = getenv("MONGODB_URI");
    if (!uristr)
    {
        uristr = "mongodb://localhost:27017";
    }
    mongoc_init();

    MONGOC_DEBUG("mongoc_get_version=%s", mongoc_get_version());
    client = mongoc_client_new(uristr);

    /* TODO */
    int64_t start = bson_get_monotonic_time();

    bson_t *ping = BCON_NEW("ping", BCON_INT32(1));
    bson_error_t error;
    while (true)
    {
        mongoc_client_command_simple(client, "admin", ping, NULL, NULL, &error);
        int64_t now = bson_get_monotonic_time();
        int64_t diff = now - start;
        if (diff >= 1000 * 1000)
        {
            break;
        }
    }

    bson_destroy(ping);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}
