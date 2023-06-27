// Q: What happens if `w`:0 is sent in runCommand directly?
// A:
//    The C driver does not parse "writeConcern" from the command.
//    Instead, the command is sent as-is without the `moreToCome` flag set.
//    The command is sent with the `{ "writeConcern": { "w": 0 } }`, and the server (7.0.0-rc0) replies.
//    The server reply for an insert contains `{ "n" : 0, "ok": 1.0 }`.

#include <mongoc/mongoc.h>
#include "../../util/monitoring.h"

#define ASSERT(stmt, ...)                                                            \
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
    mongoc_init();

    char *uristr = getenv("MONGODB_URI") ? getenv("MONGODB_URI") : "mongodb://localhost:27017";
    mongoc_client_t *client = mongoc_client_new(uristr);
    {
        mongoc_apm_callbacks_t *cbs = mongoc_apm_callbacks_new();
        set_command_monitors(cbs, false);
        mongoc_client_set_apm_callbacks(client, cbs, NULL);
        mongoc_apm_callbacks_destroy(cbs);
    }
    bson_error_t error;
    const char *cmd_str = BSON_STR({
        "insert" : "coll",
        "documents" : [ {"x" : 0} ],
        "writeConcern" : {"w" : 0}
    });
    bson_t *cmd = bson_new_from_json((const uint8_t *)cmd_str, -1, &error);
    ASSERT(cmd, "error constructing cmd: %s", error.message);
    bson_t reply;
    bool ok = mongoc_client_command_simple(client, "db", cmd, NULL /* read prefs */, &reply, &error);
    ASSERT(ok, "error sending command: %s", error.message);
    char *reply_str = bson_as_json(&reply, NULL /* length */);
    printf("got reply: %s\n", reply_str);

    bson_free(reply_str);
    bson_destroy(&reply);
    bson_destroy(cmd);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}

/*
Sample output:

2023/06/27 16:05:38.0136: [220948]:    DEBUG:       mongoc: Command insert started on localhost: { "insert" : "coll", "documents" : [ { "x" : 0 } ], "writeConcern" : { "w" : 0 }, "$db" : "db", "$readPreference" : { "mode" : "primaryPreferred" }, "lsid" : { "id" : { "$binary" : { "base64" : "o5CFxXHRQlariuD1hE486A==", "subType" : "04" } } }, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1687896328, "i" : 1 } }, "signature" : { "hash" : { "$binary" : { "base64" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "subType" : "00" } }, "keyId" : 0 } } }
2023/06/27 16:05:38.0168: [220948]:    DEBUG:       mongoc: Command insert succeeded: { "n" : 0, "ok" : 1.0, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1687896338, "i" : 2 } }, "signature" : { "hash" : { "$binary" : { "base64" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "subType" : "00" } }, "keyId" : 0 } }, "operationTime" : { "$timestamp" : { "t" : 1687896338, "i" : 2 } } }
got reply: { "n" : 0, "ok" : 1.0, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1687896338, "i" : 2 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } }, "operationTime" : { "$timestamp" : { "t" : 1687896338, "i" : 2 } } }
2023/06/27 16:05:38.0169: [220948]:    DEBUG:       mongoc: Command endSessions started on localhost: { "endSessions" : [ { "id" : { "$binary" : { "base64" : "o5CFxXHRQlariuD1hE486A==", "subType" : "04" } } } ], "$db" : "admin", "$readPreference" : { "mode" : "primaryPreferred" }, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1687896338, "i" : 2 } }, "signature" : { "hash" : { "$binary" : { "base64" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "subType" : "00" } }, "keyId" : 0 } } }
2023/06/27 16:05:38.0169: [220948]:    DEBUG:       mongoc: Command endSessions succeeded: { "ok" : 1.0, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1687896338, "i" : 2 } }, "signature" : { "hash" : { "$binary" : { "base64" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "subType" : "00" } }, "keyId" : 0 } }, "operationTime" : { "$timestamp" : { "t" : 1687896338, "i" : 2 } } }
*/
