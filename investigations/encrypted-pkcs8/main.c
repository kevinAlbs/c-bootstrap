#include <mongoc/mongoc.h>

/*

Test using an encrypted PKCS#8 client certificate.

Server is started with:

/Users/kevin.albertson/bin/mongodl/archive/8.0.0/mongodb-macos-aarch64-enterprise-8.0.0/bin/mongod \
    --tlsCAFile=/Users/kevin.albertson/code/drivers-evergreen-tools/.evergreen/x509gen/ca.pem \
    --tlsCertificateKeyFile=/Users/kevin.albertson/code/drivers-evergreen-tools/.evergreen/x509gen/server.pem \
    --tlsMode=requireTLS \
    --dbpath .menv

Test certificates are from:
https://github.com/mongodb-labs/drivers-evergreen-tools/tree/9be5587c7c876acbf2072b9bc2eab3cec78478b1/.evergreen/x509gen
*/

#define CERTPATH "/Users/kevin.albertson/code/drivers-evergreen-tools/.evergreen/x509gen"
int main()
{
    mongoc_client_t *client;
    mongoc_init();

    char *uristr = "mongodb://localhost:27017/"
                   "?tls=true"
                   "&tlsCertificateKeyFile=" CERTPATH "/client-pkcs8-encrypted.pem"
                   "&tlsCertificateKeyFilePassword=password"
                   "&tlsCAFile=" CERTPATH "/ca.pem";

    MONGOC_DEBUG("mongoc_get_version=%s", mongoc_get_version());
    client = mongoc_client_new(uristr);

    bson_t cmd = BSON_INITIALIZER;
    BSON_APPEND_INT32(&cmd, "ping", 1);
    bson_t reply;
    bson_error_t error;
    bool ok = mongoc_client_command_simple(client, "db", &cmd, NULL, &reply, &error);
    if (!ok)
    {
        printf("got error: %s\n", error.message);
        abort();
    }
    char *reply_str = bson_as_relaxed_extended_json(&reply, NULL);
    printf("got reply: %s\n", reply_str);
    bson_destroy(&cmd);

    mongoc_client_destroy(client);
    mongoc_cleanup();
}
