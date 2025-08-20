// Q: Is passing "--idleShutdownTimeoutSecs 123" treated as one arg to mongocryptd? A: Yes.
//
#include <mongoc/mongoc.h>

int main () {
    mongoc_init ();
    
    const char* spawn_path = "/Users/kevin.albertson/bin/mongodl/archive/8.0.0/mongodb-macos-aarch64-enterprise-8.0.0/bin/";
    mongoc_client_t* client = mongoc_client_new ("mongodb://localhost:27017");
    bson_error_t error;
    bson_t *kms_providers = BCON_NEW ("aws", "{", "accessKeyId", "TODO", "secretAccessKey", "TODO", "}");
    mongoc_auto_encryption_opts_t *opts =  mongoc_auto_encryption_opts_new();
    mongoc_auto_encryption_opts_set_keyvault_namespace (opts, "db", "keyvault");
    mongoc_auto_encryption_opts_set_kms_providers (opts, kms_providers);

    // Fails to spawn mongocryptd:
    // bson_t *extra = BCON_NEW ("mongocryptdSpawnArgs", "[", "--idleShutdownTimeoutSecs 123", "]", "mongocryptdSpawnPath", BCON_UTF8 (spawn_path));

    // Succeeds at spawning mongocryptd:
    bson_t *extra = BCON_NEW ("mongocryptdSpawnArgs", "[", "--idleShutdownTimeoutSecs", "123", "]", "mongocryptdSpawnPath", BCON_UTF8 (spawn_path));

    mongoc_auto_encryption_opts_set_extra (opts, extra);
    bool ok = mongoc_client_enable_auto_encryption (client, opts, &error);
    if (!ok) {
        fprintf (stderr, "Failed to enable auto encryption: %s\n", error.message);
        return 1;
    }
    bson_destroy (kms_providers);
    mongoc_auto_encryption_opts_destroy (opts);
    bson_destroy (extra);
    mongoc_client_destroy (client);
    mongoc_cleanup ();
}
