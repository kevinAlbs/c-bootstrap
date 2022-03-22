// Q: Can a different batchSize be set on 'aggregate' and 'getMore' in a change stream?
// A: No. Setting the 'batchSize' option in a change stream applies to both 'aggregate' and 'getMore'.
#include <mongoc/mongoc.h>
#include "../../util/monitoring.h"

int main () {
    char *uristr;
    mongoc_client_t *client;
    mongoc_apm_callbacks_t *cbs;
    mongoc_collection_t *coll;
    bson_t *pipeline;
    bson_t *csopts;
    mongoc_change_stream_t *changestream;

    uristr = getenv ("MONGODB_URI");
    if (!uristr) {
        uristr = "mongodb://localhost:27017";
    }
    mongoc_init ();

    MONGOC_DEBUG ("mongoc_get_version=%s", mongoc_get_version ());

    client = mongoc_client_new (uristr);
    cbs = mongoc_apm_callbacks_new ();
    set_command_monitors (cbs, true);
    mongoc_client_set_apm_callbacks (client, cbs, NULL /* context */);
    coll = mongoc_client_get_collection (client, "db", "coll");    
    pipeline = bson_new ();
    csopts = BCON_NEW ("batchSize", BCON_INT32(1));
    changestream = mongoc_collection_watch (coll, pipeline, csopts);

    while (true) {
        const bson_t* got;
        char *gotstr;
        MONGOC_DEBUG ("iterating change stream...");
        if (mongoc_change_stream_next (changestream, &got)) {
            gotstr = bson_as_json (got, NULL);
            MONGOC_DEBUG ("got: %s", gotstr);
            bson_free (gotstr);
        }
    }
    
    mongoc_change_stream_destroy (changestream);
    bson_destroy (csopts);
    bson_destroy (pipeline);
    mongoc_collection_destroy (coll);
    mongoc_apm_callbacks_destroy (cbs);
    mongoc_client_destroy (client);

    mongoc_cleanup ();
}

/*
Sample output:
2022/03/20 11:02:49.0774: [7544389]:    DEBUG:       mongoc: mongoc_get_version=1.20.0-pre
2022/03/20 11:02:49.0784: [7544389]:    DEBUG:       mongoc: Command aggregate started on localhost: { "aggregate" : "coll", "pipeline" : [ { "$changeStream" : { "fullDocument" : "default" } } ], "cursor" : { "batchSize" : 1 }, "$db" : "db", "lsid" : { "id" : { "$binary" : { "base64" : "anGQ/8RNQvSYUpeJ3xagVg==", "subType" : "04" } } }, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1647788565, "i" : 1 } }, "signature" : { "hash" : { "$binary" : { "base64" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "subType" : "00" } }, "keyId" : 0 } } }
2022/03/20 11:02:49.0795: [7544389]:    DEBUG:       mongoc: iterating change stream...
2022/03/20 11:02:49.0795: [7544389]:    DEBUG:       mongoc: Command getMore started on localhost: { "getMore" : 3987455177334131703, "collection" : "coll", "batchSize" : 1, "$db" : "db", "lsid" : { "id" : { "$binary" : { "base64" : "anGQ/8RNQvSYUpeJ3xagVg==", "subType" : "04" } } }, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1647788568, "i" : 1 } }, "signature" : { "hash" : { "$binary" : { "base64" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "subType" : "00" } }, "keyId" : 0 } } }
*/