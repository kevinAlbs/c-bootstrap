#include <mongoc/mongoc.h>

#include "../../util/monitoring.h"

#include <pthread.h>

void* thread_fn (void* arg) {
    mongoc_client_pool_t *pool = (mongoc_client_pool_t*) arg;
    int progress_counter = 0;

    while (true) {
        mongoc_client_t *client;
        bson_error_t error;
        bson_t *ping;
        
        client = mongoc_client_pool_pop (pool);
        ping = BCON_NEW ("ping", BCON_INT32(1));

        if (!mongoc_client_command_simple (client, "db", ping, NULL /* read prefs */, NULL /* reply */, &error)) {
            MONGOC_ERROR ("error in ping: %s", error.message);
            abort ();
        }
        bson_destroy (ping);
        mongoc_client_pool_push (pool, client);
        if (progress_counter % 10000 == 0) {
            MONGOC_DEBUG ("thread sent %d 'ping' commands", progress_counter);
        }
        progress_counter++;
    }

    return NULL;
}

int main () {
    const char *uristr;
    mongoc_uri_t *uri;
    mongoc_client_pool_t *pool;
    pthread_t threads[2];

    mongoc_init ();

    uristr = getenv ("MONGODB_URI");
    if (!uristr) {
        uristr = "mongodb://user:password@localhost:27017,localhost:27018";
    }

    uri = mongoc_uri_new (uristr);
    BSON_ASSERT (uri);

    pool = mongoc_client_pool_new (uri);
    BSON_ASSERT (pool);

    // For monitoring.
    // {
    //     mongoc_apm_callbacks_t *cbs;
    //     cbs = mongoc_apm_callbacks_new ();
    //     set_command_monitors (cbs, false /* started only */);
    //     mongoc_client_pool_set_apm_callbacks (pool, cbs, NULL /* context */);
    //     mongoc_apm_callbacks_destroy (cbs);
    // }

    pthread_create (threads + 0, NULL /* attr */, thread_fn, pool);
    pthread_create (threads + 1, NULL /* attr */, thread_fn, pool);

    pthread_join (threads[0], NULL /* retval */);
    pthread_join (threads[1], NULL /* retval */);

    mongoc_uri_destroy (uri);
    mongoc_client_pool_destroy (pool);

    mongoc_cleanup ();
}
