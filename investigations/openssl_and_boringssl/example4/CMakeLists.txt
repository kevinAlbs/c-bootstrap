cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(openssl_and_boringssl LANGUAGES C)
add_library(poc_boringssl.out SHARED poc_boringssl.c)
# Pass --default-symver to opt into versioning symbols from SONAME.
target_link_options(poc_boringssl.out PRIVATE LINKER:--default-symver)
# Set SOVERSION.
set_target_properties(poc_boringssl.out PROPERTIES SOVERSION 1 )
add_library(poc_tensorflow.out SHARED poc_tensorflow.c)
target_link_libraries (poc_tensorflow.out poc_boringssl.out)

add_library(poc_openssl.out SHARED poc_openssl.c)
# Pass --default-symver to opt into versioning symbols from SONAME.
target_link_options(poc_openssl.out PRIVATE LINKER:--default-symver)
# Set SOVERSION.
set_target_properties(poc_openssl.out PROPERTIES SOVERSION 1 )

add_library(poc_mongoc.out SHARED poc_mongoc.c)
target_link_libraries(poc_mongoc.out poc_openssl.out)
add_executable (app.out app.c)
target_link_libraries(app.out poc_tensorflow.out poc_mongoc.out)
