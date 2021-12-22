cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(help30191 LANGUAGES C)
# find_package(mongoc-1.0 REQUIRED)
# target_link_libraries(main.out mongo::mongoc_static)
add_executable (main.out main.c)

include (FindOpenSSL)
if (NOT "${OPENSSL_FOUND}")
    message (FATAL_ERROR "Unable to find OpenSSL")
endif()

target_link_libraries (main.out OpenSSL::Crypto OpenSSL::SSL)