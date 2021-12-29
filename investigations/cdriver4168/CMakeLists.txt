cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(cdriver4168 LANGUAGES C)
find_package(mongoc-1.0 REQUIRED)
add_executable (main.out main.c)
target_link_libraries(main.out mongo::mongoc_shared)

add_executable (repro.out repro.c)
target_link_libraries(repro.out mongo::mongoc_shared)