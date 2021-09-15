// Q: How long would it take for a server to get 32 bit overflow on the server connectionID?
// A: It takes ~1ms to create and handshake a local connection. So about 24 days.
// Even with many threads this probably is not feasible to test in one sitting with one machine.
#include <mongoc/mongoc.h>

#define ITERATIONS 1000

static void print_connectionid (bson_t* reply) {
    bson_iter_t iter;
    int32_t connectionid;

    if (!bson_iter_init_find (&iter, reply, "connectionId")) {
        MONGOC_DEBUG ("reply did not contain connectionId");
    } else if (BSON_ITER_HOLDS_INT32 (&iter)) {
        connectionid = bson_iter_int32 (&iter);
        MONGOC_DEBUG ("connectionId: %" PRId32, connectionid);
    } else {
        MONGOC_DEBUG ("unexpected type for connectionId: %d", (int) bson_iter_type (&iter));
    }
}

int main () {
    bson_t *cmd;
    mongoc_client_t *client;
    bson_t reply;
    int i;
    int64_t start_time, end_time;
    bool ok;
    bson_error_t error;

    mongoc_init ();

    
    cmd = BCON_NEW ("hello", BCON_INT32(1));
    start_time = bson_get_monotonic_time ();
    for (i = 0; i < ITERATIONS; i++) {
        client = mongoc_client_new ("mongodb://localhost:27017");
        bson_t *local_reply = NULL;
        if (i == ITERATIONS - 1) {
            local_reply = &reply;
        }
        ok = mongoc_client_command_simple (client, "admin", cmd, NULL /* read prefs */, local_reply, &error);
        mongoc_client_destroy (client);
    }
    print_connectionid (&reply);
    bson_destroy (&reply);
    end_time = bson_get_monotonic_time ();

    MONGOC_DEBUG ("time taken: %fms", (end_time - start_time) / (1000.0 * 1000.0));

    bson_destroy (cmd);
    mongoc_cleanup ();

}
