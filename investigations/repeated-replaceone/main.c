/*
 Run mongoc_collection_replace_one repeatedly.

 Sample output:
 2022/08/17 08:52:07.0779: [11290574]:    DEBUG:       mongoc: running mongoc_collection_replace_one repeatedly ... begin
 2022/08/17 08:52:07.0780: [11290574]:    DEBUG:       mongoc: localhost:27017 heartbeat started
 2022/08/17 08:52:07.0789: [11290574]:    DEBUG:       mongoc: localhost:27017 heartbeat succeeded: { "helloOk" : true, "topologyVersion" : { "processId" : { "$oid" : "62f677160e66223c9ca15818" }, "counter" : { "$numberLong" : "6" } }, "hosts" : [ "localhost:27017" ], "setName" : "rs0", "setVersion" : { "$numberInt" : "1" }, "ismaster" : true, "secondary" : false, "primary" : "localhost:27017", "me" : "localhost:27017", "electionId" : { "$oid" : "7fffffff0000000000000002" }, "lastWrite" : { "opTime" : { "ts" : { "$timestamp" : { "t" : 1660740726, "i" : 1 } }, "t" : { "$numberLong" : "2" } }, "lastWriteDate" : { "$date" : { "$numberLong" : "1660740726000" } }, "majorityOpTime" : { "ts" : { "$timestamp" : { "t" : 1660740726, "i" : 1 } }, "t" : { "$numberLong" : "2" } }, "majorityWriteDate" : { "$date" : { "$numberLong" : "1660740726000" } } }, "maxBsonObjectSize" : { "$numberInt" : "16777216" }, "maxMessageSizeBytes" : { "$numberInt" : "48000000" }, "maxWriteBatchSize" : { "$numberInt" : "100000" }, "localTime" : { "$date" : { "$numberLong" : "1660740727788" } }, "logicalSessionTimeoutMinutes" : { "$numberInt" : "30" }, "connectionId" : { "$numberInt" : "555" }, "minWireVersion" : { "$numberInt" : "0" }, "maxWireVersion" : { "$numberInt" : "18" }, "readOnly" : false, "ok" : { "$numberDouble" : "1.0" }, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1660740726, "i" : 1 } }, "signature" : { "hash" : { "$binary" : { "base64" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "subType" : "00" } }, "keyId" : { "$numberLong" : "0" } } }, "operationTime" : { "$timestamp" : { "t" : 1660740726, "i" : 1 } } }
 2022/08/17 08:52:08.0346: [11290574]:    DEBUG:       mongoc: running mongoc_collection_replace_one repeatedly ... end
*/

#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>

static BSON_INLINE void
server_heartbeat_started(const mongoc_apm_server_heartbeat_started_t *event)
{
    MONGOC_DEBUG(
        "%s heartbeat started",
        mongoc_apm_server_heartbeat_started_get_host(event)->host_and_port);
}

static BSON_INLINE void
server_heartbeat_succeeded(
    const mongoc_apm_server_heartbeat_succeeded_t *event)
{
    char *reply;

    reply = bson_as_canonical_extended_json(
        mongoc_apm_server_heartbeat_succeeded_get_reply(event), NULL);

    MONGOC_DEBUG(
        "%s heartbeat succeeded: %s",
        mongoc_apm_server_heartbeat_succeeded_get_host(event)->host_and_port,
        reply);

    bson_free(reply);
}

static BSON_INLINE void
server_heartbeat_failed(const mongoc_apm_server_heartbeat_failed_t *event)
{
    bson_error_t error;

    mongoc_apm_server_heartbeat_failed_get_error(event, &error);

    MONGOC_DEBUG(
        "%s heartbeat failed: %s",
        mongoc_apm_server_heartbeat_failed_get_host(event)->host_and_port,
        error.message);
}

static BSON_INLINE void
set_sdam_monitors(mongoc_apm_callbacks_t *cbs)
{
    mongoc_apm_set_server_heartbeat_started_cb(cbs, server_heartbeat_started);
    mongoc_apm_set_server_heartbeat_succeeded_cb(cbs,
                                                 server_heartbeat_succeeded);
    mongoc_apm_set_server_heartbeat_failed_cb(cbs, server_heartbeat_failed);
}

int main(int argc, char *argv[])
{
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_error_t error;
    char *uristr;
    mongoc_apm_callbacks_t *callbacks = mongoc_apm_callbacks_new();

    uristr = getenv("MONGODB_URI");
    if (!uristr)
    {
        uristr = "mongodb://localhost:27017";
    }

    mongoc_init();

    client = mongoc_client_new(uristr);
    if (!client)
    {
        MONGOC_DEBUG("error in mongoc_client_new_from_uri");
        return EXIT_FAILURE;
    }

    set_sdam_monitors(callbacks);
    if (!mongoc_client_set_apm_callbacks(
            client, callbacks, NULL /* context */))
    {
        MONGOC_DEBUG("error in mongoc_client_set_apm_callbacks");
        return EXIT_FAILURE;
    }
    mongoc_client_set_error_api(client, 2);

    collection = mongoc_client_get_collection(client, "db", "coll");
    bson_t *selector = bson_new();
    bson_t *replacement = BCON_NEW("foo", "bar");
    MONGOC_DEBUG("running mongoc_collection_replace_one repeatedly ... begin");
    for (int i = 0; i < 100.0; i++)
    {
        bool ok = mongoc_collection_replace_one(collection,
                                                selector,
                                                replacement,
                                                NULL /* opts */,
                                                NULL /* reply */,
                                                &error);
        if (!ok)
        {
            MONGOC_DEBUG("error in mongoc_collection_replace_one: %s\n",
                         error.message);
            return EXIT_FAILURE;
        }
    }
    MONGOC_DEBUG("running mongoc_collection_replace_one repeatedly ... end");

    bson_destroy(selector);
    bson_destroy(replacement);
    mongoc_collection_destroy(collection);
    mongoc_apm_callbacks_destroy(callbacks);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return EXIT_SUCCESS;
}
