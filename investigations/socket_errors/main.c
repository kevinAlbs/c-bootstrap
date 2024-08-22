// Q: What error is returned on network timeout? On connection close?
// A:
//  Both cases log a warning: "stream: Failed to buffer 4 bytes"
//  Both cases return an error: "Failed to read 4 bytes: socket error or timeout".

#include <mongoc/mongoc.h>

#define ABORT(...)                                    \
    printf("Error occurred on line: %d\n", __LINE__); \
    printf(__VA_ARGS__);                              \
    printf("\n");                                     \
    abort();
#define ASSERT(stmt) BSON_ASSERT(stmt);

int main()
{
    mongoc_init();

    MONGOC_DEBUG("mongoc_get_version=%s", mongoc_get_version());
    // Set a socket timeout of 500ms.
    char *uristr = "mongodb://localhost:27017/?socketTimeoutMS=500";
    mongoc_uri_t *uri = mongoc_uri_new(uristr);
    mongoc_client_pool_t *pool = mongoc_client_pool_new(uri);
    mongoc_client_t *client = mongoc_client_pool_pop(pool);

    bson_error_t error;
    // Test network timeout.
    {
        // Configure failpoint to block for 600ms.
        {
            const char *cmd_str = BSON_STR({
                "configureFailPoint" : "failCommand",
                "mode" : {"times" : 1},
                "data" : {"blockConnection" : true, "blockTimeMS" : 600, "failCommands" : ["ping"]}
            });
            bson_t *cmd = bson_new_from_json((const uint8_t *)cmd_str, -1, &error);
            if (!cmd)
            {
                ABORT("failed to construct command: %s", error.message);
            }
            bool ok = mongoc_client_command_simple(client, "admin", cmd, NULL, NULL, &error);
            if (!ok)
            {
                ABORT("failed to configure failpoint: %s", error.message);
            }
            bson_destroy(cmd);
        }

        // Send ping.
        {
            bson_t *ping = BCON_NEW("ping", BCON_INT32(1));
            bool ok = mongoc_client_command_simple(client, "admin", ping, NULL, NULL, &error);
            BSON_ASSERT(!ok);
            const char *expect = "Failed to read 4 bytes: socket error or timeout";
            if (!strstr(error.message, expect))
            {
                ABORT("Expected error to contain '%s', but got '%s'", expect, error.message);
            }
            bson_destroy(ping);
        }
    }

    // Test network error.
    {
        // Configure failpoint to close connection.
        {
            const char *cmd_str = BSON_STR({
                "configureFailPoint" : "failCommand",
                "mode" : {"times" : 1},
                "data" : {"closeConnection" : true, "failCommands" : ["ping"]}
            });
            bson_t *cmd = bson_new_from_json((const uint8_t *)cmd_str, -1, &error);
            if (!cmd)
            {
                ABORT("failed to construct command: %s", error.message);
            }
            bool ok = mongoc_client_command_simple(client, "admin", cmd, NULL, NULL, &error);
            if (!ok)
            {
                ABORT("failed to configure failpoint: %s", error.message);
            }
            bson_destroy(cmd);
        }

        // Send ping.
        {
            bson_t *ping = BCON_NEW("ping", BCON_INT32(1));
            bool ok = mongoc_client_command_simple(client, "admin", ping, NULL, NULL, &error);
            BSON_ASSERT(!ok);
            const char *expect = "Failed to read 4 bytes: socket error or timeout";
            if (!strstr(error.message, expect))
            {
                ABORT("Expected error to contain '%s', but got '%s'", expect, error.message);
            }
            bson_destroy(ping);
        }
    }

    mongoc_client_pool_push(pool, client);
    mongoc_client_pool_destroy(pool);
    mongoc_uri_destroy(uri);
    mongoc_cleanup();
}
