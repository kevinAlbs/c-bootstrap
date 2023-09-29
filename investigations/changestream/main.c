#include <mongoc/mongoc.h>

#include "../../util/monitoring.h"

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

    // Print command started events.
    {
        mongoc_apm_callbacks_t *cbs = mongoc_apm_callbacks_new();
        set_command_monitors(cbs, true);
        BSON_ASSERT(mongoc_client_set_apm_callbacks(client, cbs, NULL /* context */));
    }

    mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");

    // Create a change stream with no arguments. Observe the printed command started event.
    {
        bson_t *pipeline = bson_new();
        mongoc_change_stream_t *cs = mongoc_collection_watch(coll, pipeline, NULL /* opts */);
        // Example printed command:
        // { "aggregate" : "coll", "pipeline" : [ { "$changeStream" : {  } } ], "cursor" : {  }, "$db" : "db", "$readPreference" : { "mode" : "primaryPreferred" }, "lsid" : { "id" : { "$binary" : { "base64" : "hgXUgmysS0izVLIDFWBt/w==", "subType" : "04" } } }, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1695990741, "i" : 1 } }, "signature" : { "hash" : { "$binary" : { "base64" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "subType" : "00" } }, "keyId" : 0 } } }
        mongoc_change_stream_destroy(cs);
        bson_destroy(pipeline);
    }

    mongoc_collection_destroy(coll);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}
