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
    bool ok = mongoc_client_command_simple(client, "admin", ping, NULL /* read prefs */, NULL /* reply */, &error);
    if (!ok)
    {
        fprintf(stderr, "Error on 'ping': %s\n", error.message);
    }

    mongoc_client_destroy(client);
    mongoc_cleanup();
}

/* Inspecting server logs shows two handshakes:

{"t":{"$date":"2023-12-13T13:00:39.283-05:00"},"s":"I",  "c":"NETWORK",  "id":51800,   "ctx":"conn13","msg":"client metadata","attr":{"remote":"127.0.0.1:49998","client":"conn13","doc":{"driver":{"name":"mongoc","version":"1.24.3"},"os":{"type":"Darwin","name":"macOS","version":"21.6.0","architecture":"arm64"},"platform":"cfg=0x0300deaa65 posix=200112 stdc=199901 CC=clang 14.0.0 (clang-1400.0.29.202) CFLAGS=\"\" LDFLAGS=\"\""}}}
{"t":{"$date":"2023-12-13T13:00:39.374-05:00"},"s":"I",  "c":"NETWORK",  "id":51800,   "ctx":"conn14","msg":"client metadata","attr":{"remote":"127.0.0.1:50000","client":"conn14","doc":{"driver":{"name":"mongoc","version":"1.25.0"},"os":{"type":"Darwin","name":"macOS","version":"21.6.0","architecture":"arm64"},"platform":"cfg=0x0383d6aa65 posix=200112 stdc=199901 CC=clang 14.0.0 (clang-1400.0.29.202) CFLAGS=\"\" LDFLAGS=\"\""}}}

*/
