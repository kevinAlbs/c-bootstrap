cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(help30695 LANGUAGES C CXX)
find_package(mongoc-1.0 REQUIRED)
add_executable (main.out main.c)
target_link_libraries(main.out mongo::mongoc_shared)

add_executable (colby.out repro.cpp)
target_link_libraries (colby.out mongo::mongoc_shared)