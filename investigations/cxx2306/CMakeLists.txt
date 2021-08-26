cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(HELLO_WORLD LANGUAGES C)

find_package(mongoc-1.0 REQUIRED)

add_executable (cxx2306 cxx2306.c)
target_link_libraries(cxx2306 mongo::mongoc_shared)