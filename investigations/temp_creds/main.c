// Q: Can sessionToken be passed to CSFLE in the C driver?
// A: Yes.
#include <mongoc/mongoc.h>

int main()
{
    char *mongodb_uri = getenv("MONGODB_URI");
    char *aws_session_token = getenv("AWS_SESSION_TOKEN");
    char *aws_access_key_id = getenv("AWS_ACCESS_KEY_ID");
    char *aws_secret_access_key = getenv("AWS_SECRET_ACCESS_KEY");
    if (!mongodb_uri || !aws_access_key_id || !aws_secret_access_key || !aws_session_token)
    {
        MONGOC_ERROR("Error: set environment variables: MONGODB_URI, AWS_SESSION_TOKEN, AWS_ACCESS_KEY_ID, AWS_SECRET_ACCESS_KEY");
        return EXIT_FAILURE;
    }

    mongoc_init();

    mongoc_client_t *keyvault_client = mongoc_client_new(mongodb_uri);
    MONGOC_DEBUG("libmongoc version: %s", mongoc_get_version());

    mongoc_client_encryption_t *ce;
    mongoc_client_encryption_opts_t *ceopts;

    ceopts = mongoc_client_encryption_opts_new();
    mongoc_client_encryption_opts_set_keyvault_client(ceopts, keyvault_client);
    mongoc_client_encryption_opts_set_keyvault_namespace(ceopts, "keyvault", "datakeys");

    bson_t *kms_providers = BCON_NEW(
        "aws", "{",
        "accessKeyId", BCON_UTF8(aws_access_key_id),
        "secretAccessKey", BCON_UTF8(aws_secret_access_key),
        "sessionToken", BCON_UTF8(aws_session_token),
        "}");

    mongoc_client_encryption_opts_set_kms_providers(ceopts, kms_providers);

    bson_error_t error;
    ce = mongoc_client_encryption_new(ceopts, &error);
    if (!ce)
    {
        MONGOC_ERROR("Error in mongoc_client_encryption_new: %s", error.message);
        return EXIT_FAILURE;
    }

    mongoc_client_encryption_datakey_opts_t *dkopts;
    dkopts = mongoc_client_encryption_datakey_opts_new();
    bson_t *masterkey = BCON_NEW(
        "region", "us-east-1",
        "key", "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0");
    mongoc_client_encryption_datakey_opts_set_masterkey(dkopts, masterkey);

    bson_value_t keyid;
    if (!mongoc_client_encryption_create_datakey(ce, "aws", dkopts, &keyid, &error))
    {
        MONGOC_ERROR("Error in mongoc_client_encryption_create_datakey: %s", error.message);
        return EXIT_FAILURE;
    }
    MONGOC_DEBUG("Created key\n");
    bson_value_destroy(&keyid);
    bson_destroy(masterkey);
    mongoc_client_encryption_datakey_opts_destroy(dkopts);
    mongoc_client_encryption_destroy(ce);
    bson_destroy(kms_providers);
    mongoc_client_encryption_opts_destroy(ceopts);
    mongoc_client_destroy(keyvault_client);

    mongoc_cleanup();
}
