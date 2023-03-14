#include <mongoc/mongoc.h>

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

    bson_t *ping = BCON_NEW("ping", BCON_INT32(1));
    bson_error_t error;
    bool ok = mongoc_client_command_simple(client, "db", ping, NULL, NULL, &error);
    if (!ok)
    {
        printf("unexpected error: %s\n", error.message);
        return EXIT_FAILURE;
    }

    mongoc_client_destroy(client);
    mongoc_cleanup();
}

/*
Here is an example formatted output from mongod logs for the C driver 1.23.2:
{
    "t": {
        "$date": "2023-03-14T08:59:59.725-04:00"
    },
    "s": "I",
    "c": "NETWORK",
    "id": 51800,
    "ctx": "conn1013",
    "msg": "client metadata",
    "attr": {
        "remote": "127.0.0.1:62602",
        "client": "conn1013",
        "doc": {
            "driver": {
                "name": "mongoc",
                "version": "1.23.2"
            },
            "os": {
                "type": "Darwin",
                "name": "macOS",
                "version": "21.6.0",
                "architecture": "arm64"
            },
            "platform": "cfg=0x0300d6aa65 posix=200112 stdc=201710 CC=clang 13.1.6 (clang-1316.0.21.2.5) CFLAGS=\"\" LDFLAGS=\"\""
        }
    }
}
*/