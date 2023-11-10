#include <mongoc/mongoc.h>

int main()
{
    bson_error_t error;
    char *uristr;

    uristr = getenv("MONGODB_URI");
    if (!uristr)
    {
        uristr = "mongodb://localhost:27017";
    }
    mongoc_init();

    MONGOC_DEBUG("mongoc_get_version=%s", mongoc_get_version());

    mongoc_uri_t *uri = mongoc_uri_new(uristr);
    mongoc_uri_set_option_as_int32(uri, MONGOC_URI_SOCKETTIMEOUTMS, -1);
    mongoc_client_t *client = mongoc_client_new_from_uri_with_error(uri, &error);
    if (!client)
    {
        fprintf(stderr, "error creating client: %s", error.message);
        abort();
    }
    bson_t *ping = BCON_NEW("ping", BCON_INT32(1));
    bool ok = mongoc_client_command_simple(client, "db", ping, NULL, NULL, &error);
    if (!ok)
    {
        // Results in error:
        // error in ping: Failed to send "saslContinue" command with database "admin": timeout_msec value 4294967295 exceeds supported 32-bit range
        fprintf(stderr, "error in ping: %s", error.message);
        abort();
    }
    mongoc_client_destroy(client);
    mongoc_uri_destroy(uri);

    mongoc_cleanup();
}
