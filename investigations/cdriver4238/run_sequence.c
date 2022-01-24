/* Run the sequence of commands: ping, ping, insert, ping, ping.
 * This was used to test a recovery scenario.
 * To run the test:
 * - Link to the C driver with mock_cluster_time.patch applied.
 *     The patch mocks the $clusterTime for the first "ping" command.
 *       The mocked $clusterTime increments $clusterTime.clusterTime by 1.
 *       The mocked $clusterTime sets $clusterTime.keyId to 0.
 */
#include <mongoc/mongoc.h>

#include "../../util/monitoring.h"

int main () {
    const char *uristr;
    mongoc_uri_t *uri;
    mongoc_client_pool_t *pool_ping;
    mongoc_client_pool_t *pool_insert;
    int i;

    mongoc_init ();

    uristr = getenv ("MONGODB_URI");
    if (!uristr) {
        uristr = "mongodb://user:password@localhost:27017,localhost:27018";
    }

    uri = mongoc_uri_new (uristr);
    BSON_ASSERT (uri);

    pool_ping = mongoc_client_pool_new (uri);
    BSON_ASSERT (pool_ping);

    pool_insert = mongoc_client_pool_new (uri);
    BSON_ASSERT (pool_insert);

    /* Enable monitoring */
    {
        mongoc_apm_callbacks_t *cbs;
        cbs = mongoc_apm_callbacks_new ();
        set_command_monitors (cbs, false /* started only */);
        mongoc_client_pool_set_apm_callbacks (pool_ping, cbs, NULL /* context */);
        mongoc_client_pool_set_apm_callbacks (pool_insert, cbs, NULL /* context */);
        mongoc_apm_callbacks_destroy (cbs);
    }

    /* Run ping */
    {
        mongoc_client_t *client;
        bson_error_t error;
        bson_t *ping;
        
        client = mongoc_client_pool_pop (pool_ping);
        ping = BCON_NEW ("ping", BCON_INT32 (1));

        if (!mongoc_client_command_simple (client, "db", ping, NULL /* read prefs */, NULL /* reply */, &error)) {
            MONGOC_ERROR ("error in ping: %s", error.message);
        }
        bson_destroy (ping);
        mongoc_client_pool_push (pool_ping, client);
    }
    /* Run ping */
    {
        mongoc_client_t *client;
        bson_error_t error;
        bson_t *ping;
        
        client = mongoc_client_pool_pop (pool_ping);
        ping = BCON_NEW ("ping", BCON_INT32 (1));

        if (!mongoc_client_command_simple (client, "db", ping, NULL /* read prefs */, NULL /* reply */, &error)) {
            MONGOC_ERROR ("error in ping: %s", error.message);
        }
        bson_destroy (ping);
        mongoc_client_pool_push (pool_ping, client);
    }
    /* Run insert on separate client. This increments server $clusterTime.timestamp */
    for (i = 0; i < 1; i++) {
        mongoc_client_t *client;
        bson_error_t error;
        bson_t *doc;
        mongoc_collection_t *coll;
        
        client = mongoc_client_pool_pop (pool_insert);
        doc = BCON_NEW ("x", BCON_INT32(i));

        coll = mongoc_client_get_collection (client, "db", "coll");
        if (!mongoc_collection_insert_one (coll, doc, NULL /* opts */, NULL /* reply */, &error)) {
            MONGOC_ERROR ("error in insert: %s", error.message);
        }
        mongoc_collection_destroy (coll);
        bson_destroy (doc);
        mongoc_client_pool_push (pool_insert, client);
    }
    /* Run ping */
    {
        mongoc_client_t *client;
        bson_error_t error;
        bson_t *ping;
        
        client = mongoc_client_pool_pop (pool_ping);
        ping = BCON_NEW ("ping", BCON_INT32 (1));

        if (!mongoc_client_command_simple (client, "db", ping, NULL /* read prefs */, NULL /* reply */, &error)) {
            MONGOC_ERROR ("error in ping: %s", error.message);
        }
        bson_destroy (ping);
        mongoc_client_pool_push (pool_ping, client);
    }


    mongoc_uri_destroy (uri);
    mongoc_client_pool_destroy (pool_ping);
    mongoc_client_pool_destroy (pool_insert);

    mongoc_cleanup ();
}
