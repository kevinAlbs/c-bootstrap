#include <mongoc/mongoc.h>

int main () {
    mongoc_client_t *client;
    bson_t *cmd;
    bson_error_t error;
    bool ok;

    mongoc_init ();

    mongoc_handshake_data_append ("driver_name", "driver_version", "platform");
    
    cmd = BCON_NEW ("ping", BCON_INT32 (1));
    client = mongoc_client_new ("mongodb://localhost:27017");
    ok = mongoc_client_command_simple (client, "db", cmd, NULL /* read_prefs */, NULL /* reply */, &error);
    if (!ok) {
        MONGOC_ERROR ("error on 'ping': %s", error.message);
        return EXIT_FAILURE;
    }

    mongoc_client_destroy (client);
    bson_destroy (cmd);

    mongoc_cleanup ();
}

/* Using `tail -f` on the mongod logs, I observed the following:
{"t":{"$date":"2021-09-08T09:42:43.247-04:00"},"s":"I",  "c":"NETWORK",  "id":51800,   "ctx":"conn17","msg":"client metadata","attr":{"remote":"127.0.0.1:53013","client":"conn17","doc":{"driver":{"name":"mongoc / driver_name","version":"1.18.0-pre / driver_version"},"os":{"type":"Darwin","name":"macOS","version":"20.5.0","architecture":"x86_64"},"platform":"platformcfg=0x0300d68265 posix=200112 stdc=201710 CC=clang 11.0.0 (https://github.com/llvm/llvm-project.git 176249bd6732a8044d457092ed932768724a6f06) CFLAGS=\"\" LDFLAGS=\"\""}}}
*/
