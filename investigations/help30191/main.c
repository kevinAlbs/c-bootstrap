// #include <mongoc/mongoc.h>

#include <openssl/ssl.h>
#include <openssl/opensslv.h>

void
_mongoc_openssl_init (void)
{
   SSL_CTX *ctx;

   SSL_library_init ();
   SSL_load_error_strings ();
   ERR_load_BIO_strings ();
   OpenSSL_add_all_algorithms ();

   ctx = SSL_CTX_new (SSLv23_method ());
   if (!ctx) {
      fprintf (stderr, "Failed to initialize OpenSSL.");
   }

   SSL_CTX_free (ctx);
}

int main () {
    printf ("Using OpenSSL: %s\n", OPENSSL_VERSION_TEXT);
    _mongoc_openssl_init ();
    _mongoc_openssl_init ();
    printf ("Finished initializing\n");
}

/* Sample output:
 *
 * Using OpenSSL: OpenSSL 1.0.2u  20 Dec 2019
 * Finished initializing 
 */

/*
 * OpenSSL built from source as follows:
 * 
 * wget https://www.openssl.org/source/old/1.0.2/openssl-1.0.2u.tar.gz
 * tar xf openssl-1.0.2u.tar.gz
 * cd openssl-1.0.2u
 * ./Configure --prefix=/Users/kevin.albertson/bin/openssl-1.0.2u-debug --openssldir=/Users/kevin.albertson/bin/openssl-1.0.2u/openssl-debug debug-darwin64-x86_64-cc
 * make
 * make install
 */