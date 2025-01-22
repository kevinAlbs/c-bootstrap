cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(HELP-69502 LANGUAGES C)
find_package(mongoc-1.0 REQUIRED)
find_package(Threads REQUIRED)

add_executable (benchmark-sd-copy benchmark-sd-copy.c)
target_link_libraries(benchmark-sd-copy mongo::mongoc_shared)
