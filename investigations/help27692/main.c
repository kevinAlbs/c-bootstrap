#include <mongoc/mongoc.h>
#include <unistd.h> /* sleep */
#include "../../util/monitoring.h"

int main () {
    mongoc_client_pool_t *pool;
    mongoc_client_t *client;
    mongoc_uri_t *uri;
    mongoc_apm_callbacks_t *cbs;
    char *uristr = getenv ("MONGODB_URI");
    bson_t *ping;
    int iterations = 0;

    mongoc_init ();

    if (!uristr) {
        MONGOC_ERROR ("set MONGODB_URI");
        return EXIT_FAILURE;
    }
    uri = mongoc_uri_new (uristr);
    mongoc_uri_set_option_as_int32 (uri, MONGOC_URI_SERVERSELECTIONTIMEOUTMS, 10000);
    pool = mongoc_client_pool_new (uri);
    cbs = mongoc_apm_callbacks_new ();
    set_command_monitors (cbs, false);
    set_sdam_monitors (cbs);

    mongoc_client_pool_set_apm_callbacks (pool, cbs, NULL);
    client = mongoc_client_pool_pop (pool);
    ping = BCON_NEW ("ping", BCON_INT32 (1));

    while (true) {
        bson_error_t error;
        bool ok;

        MONGOC_DEBUG ("iteration %d", iterations);
        iterations++;
        ok = mongoc_client_command_simple (client, "admin", ping, NULL /* read prefs */, NULL /* reply */, &error);
        if (!ok) {
            MONGOC_ERROR ("error on ping: %s", error.message);
            return EXIT_FAILURE;
        }

        sleep (1);
    }

    bson_destroy (ping);
    mongoc_client_pool_push (pool, client);
    mongoc_apm_callbacks_destroy (cbs);
    mongoc_client_pool_destroy (pool);
    mongoc_uri_destroy (uri);
    free (uristr);

    mongoc_cleanup ();
}
