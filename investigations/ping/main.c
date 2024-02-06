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

    bson_t *ping = BCON_NEW("ping", BCON_INT32(1));
    bson_t reply;
    bson_error_t error;
    bool ok = mongoc_client_command_simple(client, "admin", ping, NULL, &reply, &error);
    if (!ok)
    {
        printf("error: %s\n", error.message);
        char *reply_str = bson_as_canonical_extended_json(&reply, NULL);
        printf("reply: %s\n", reply_str);
    }

    /* TODO */

    mongoc_client_destroy(client);
    mongoc_cleanup();
}
