#include <mongoc/mongoc.h>
#include <stdio.h>
#include <string.h>

#define CERTPATH "/Users/kevin.albertson/code/c-bootstrap/investigations/H74576/repro/certs"

int main()

{
    mongoc_client_t *client;
    mongoc_database_t *database;
    bson_t ping;
    bson_error_t error;
    const char *uri_string = "mongodb://localhost:27017/?tls=true&tlsAllowInvalidHostnames=true";
    mongoc_ssl_opt_t ssl_opts = {0};

    // Test an encrypted PKCS#8 cert:
    {
        ssl_opts.pem_file = CERTPATH "/server/mongodb-test-server-encrypted-pkcs8.pem";
        ssl_opts.pem_pwd = "foobar";
        ssl_opts.ca_file = CERTPATH "/ia/mongodb-test-ia.crt";
    }

    mongoc_init();

    client = mongoc_client_new(uri_string);
    if (!client)
    {
        fprintf(stderr, "Failed to create MongoDB client\n");
        return 1;
    }

    mongoc_client_set_ssl_opts(client, &ssl_opts);

    bson_init(&ping);
    BSON_APPEND_INT32(&ping, "ping", 1);

    database = mongoc_client_get_database(client, "admin");
    if (!mongoc_database_command_simple(database, &ping, NULL, NULL, &error))
    {
        fprintf(stderr, "Ping failed: %s\n", error.message);
        bson_destroy(&ping);
        mongoc_database_destroy(database);
        mongoc_client_destroy(client);
        mongoc_cleanup();
        return 1;
    }

    printf("Connected and pinged MongoDB successfully over TLS.\n");

    bson_destroy(&ping);
    mongoc_database_destroy(database);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return 0;
}
