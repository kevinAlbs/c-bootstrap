#include <openssl/is_boringssl.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <stdlib.h>

int main () {
    /* Do something with BoringSSL */
    BIO* bio = BIO_new (BIO_s_mem());
    BIO_free (bio);
    
    return EXIT_SUCCESS;
}