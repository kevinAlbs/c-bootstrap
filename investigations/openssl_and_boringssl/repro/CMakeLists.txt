cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(openssl_and_boringssl LANGUAGES C)
find_package(mongoc-1.0 REQUIRED)
add_executable (main.out main.c)
target_link_libraries(main.out mongo::mongoc_shared)

set (BORINGSSL_INCLUDE "/Users/kevin.albertson/code/c-bootstrap/install/boringssl-src/include")
set (BORINGSSL_SSL_SHARED_LIB "/Users/kevin.albertson/code/c-bootstrap/install/boringssl-src/build/ssl/libssl.dylib")
set (BORINGSSL_CRYPTO_SHARED_LIB "/Users/kevin.albertson/code/c-bootstrap/install/boringssl-src/build/crypto/libcrypto.dylib")

message ("got ${BORINGSSL_SHARED_LIB}")
target_include_directories (main.out PRIVATE "${BORINGSSL_INCLUDE}")
target_link_libraries (main.out "${BORINGSSL_SSL_SHARED_LIB}")
target_link_libraries (main.out "${BORINGSSL_CRYPTO_SHARED_LIB}")

add_executable (boringssl_only.out boringssl_only.c)
target_include_directories (boringssl_only.out PRIVATE "${BORINGSSL_INCLUDE}")
target_link_libraries (boringssl_only.out "${BORINGSSL_SSL_SHARED_LIB}")
target_link_libraries (boringssl_only.out "${BORINGSSL_CRYPTO_SHARED_LIB}")