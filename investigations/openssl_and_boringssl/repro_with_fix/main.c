#include <mongoc/mongoc.h>
#include <openssl/is_boringssl.h>
#include <openssl/evp.h>
#include <openssl/bio.h>

int main () {
    mongoc_client_t *client;
    bson_t *bson;
    char *uristr;
    bson_error_t error;

    /* Do something with BoringSSL */
    BIO* bio = BIO_new (BIO_s_mem());
    BIO_free (bio);

    uristr = getenv ("MONGODB_URI");
    if (!uristr) {
        uristr = "mongodb://localhost:27017";
    }
    mongoc_init ();

    client = mongoc_client_new (uristr);
    bson = BCON_NEW ("ping", BCON_INT32(1));
    if (!mongoc_client_command_simple (client, "admin", bson, NULL /* read prefs */, NULL /* reply */, &error)) {
        MONGOC_ERROR ("error in mongoc_client_command_simple: %s", error.message);
        return EXIT_FAILURE;
    }
    mongoc_client_destroy (client);
    mongoc_cleanup ();

    
    return EXIT_SUCCESS;
}