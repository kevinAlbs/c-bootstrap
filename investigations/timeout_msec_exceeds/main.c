#include <mongoc/mongoc.h>

#define ASSERT BSON_ASSERT
#define ASSERTF(stmt, ...)                                                           \
    if (!(stmt))                                                                     \
    {                                                                                \
        fprintf(stderr, "error at statement \"%s\" on line: %d\n", #stmt, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                                                \
        fprintf(stderr, "\n");                                                       \
        abort();                                                                     \
    }                                                                                \
    else                                                                             \
        (void)0

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

    // Try calling `mongoc_cursor_set_max_await_time_ms` with a -1 value.
    {
        mongoc_client_t *client = mongoc_client_new(uristr);
        mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");
        bson_t filter = BSON_INITIALIZER;
        mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(coll, &filter, NULL /* opts */, NULL /* read prefs */);
        mongoc_cursor_set_max_await_time_ms(cursor, -1);
        const bson_t *got;
        // Iterate cursor to send command.
        mongoc_cursor_next(cursor, &got);
        ASSERTF(!mongoc_cursor_error(cursor, &error), "cursor error: %s", error.message);
        mongoc_collection_destroy(coll);
        mongoc_client_destroy(client);
    }

    // Try calling `mongoc_find_and_modify_opts_set_max_time_ms` with a -1 value.
    {
        mongoc_client_t *client = mongoc_client_new(uristr);
        mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");
        bson_t query = BSON_INITIALIZER;
        mongoc_find_and_modify_opts_t *fam_opts = mongoc_find_and_modify_opts_new();
        mongoc_find_and_modify_opts_set_max_time_ms(fam_opts, -1);
        bool ok = mongoc_collection_find_and_modify_with_opts(coll, &query, fam_opts, NULL /* reply */, &error);
        // Results in different error: "-1 value for maxTimeMS is out of range [0, 2147483647]"
        // ASSERTF(ok, "error in find_and_modify: %s", error.message);
        mongoc_find_and_modify_opts_destroy(fam_opts);
        mongoc_collection_destroy(coll);
        mongoc_client_destroy(client);
    }

    // // Try setting all URI timeouts as -1 and running a ping with authentication.
    // {
    //     mongoc_uri_t *uri = mongoc_uri_new(uristr);
    //     // mongoc_uri_set_option_as_int32(uri, MONGOC_URI_CONNECTTIMEOUTMS, -1); // Resulted in "No suitable servers found (`serverSelectionTryOnce` set)"
    //     mongoc_uri_set_option_as_int32(uri, MONGOC_URI_SOCKETTIMEOUTMS, -1);
    //     // mongoc_uri_set_option_as_int32(uri, MONGOC_URI_HEARTBEATFREQUENCYMS, -1);
    //     // mongoc_uri_set_option_as_int32(uri, MONGOC_URI_SERVERSELECTIONTIMEOUTMS, -1);
    //     // mongoc_uri_set_option_as_int32(uri, MONGOC_URI_MAXIDLETIMEMS, -1);
    //     // mongoc_uri_set_option_as_int32(uri, MONGOC_URI_WAITQUEUETIMEOUTMS, -1);
    //     // mongoc_uri_set_option_as_int32(uri, MONGOC_URI_WTIMEOUTMS, -1);
    //     // mongoc_uri_set_option_as_int32(uri, MONGOC_URI_LOCALTHRESHOLDMS, -1);
    //     // mongoc_uri_set_option_as_int32(uri, MONGOC_URI_MAXSTALENESSSECONDS, -1);

    //     mongoc_client_t *client = mongoc_client_new_from_uri_with_error(uri, &error);
    //     ASSERTF(client, "error creating client: %s", error.message);

    //     bson_t *ping = BCON_NEW("ping", BCON_INT32(1));

    //     bool ok = mongoc_client_command_simple(client, "db", ping, NULL, NULL, &error);
    //     // ASSERTF(ok, "error in ping: %s", error.message);

    //     mongoc_client_destroy(client);
    //     mongoc_uri_destroy(uri);
    // }

    // Shorter repro
    {
        mongoc_uri_t *uri = mongoc_uri_new(uristr);
        mongoc_uri_set_option_as_int32(uri, MONGOC_URI_SOCKETTIMEOUTMS, -1);
        mongoc_client_t *client = mongoc_client_new_from_uri_with_error(uri, &error);
        ASSERTF(client, "error creating client: %s", error.message);
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
    }

    mongoc_cleanup();
}
