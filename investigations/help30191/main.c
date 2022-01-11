#include <mongoc/mongoc.h>

#include <openssl/ssl.h>
#include <openssl/opensslv.h>

#include <pthread.h>

void* calls_SSL_library_init (void* arg) {
    SSL_library_init ();
    return NULL;
}

int main () {
    printf ("Using OpenSSL: %s\n", OPENSSL_VERSION_TEXT);

    // Try calling SSL_library_init twice
    // SSL_library_init ();
    // SSL_library_init ();

    // Try calling SSL_library_init before mongoc_init:
    SSL_library_init ();
    mongoc_init();
    printf ("mongoc %s\n", mongoc_get_version ());

    // Try calling mongoc_init while another thread is calling SSL_library_init.
    // {
    //     pthread_t thread;
    //     pthread_create (&thread, NULL /* attr */, calls_SSL_library_init_and_cleanup_in_loop, NULL /* arg */);
    //     mongoc_init ();
    //     pthread_join (thread, NULL /* return */);
    // }
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