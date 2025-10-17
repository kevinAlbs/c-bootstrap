cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(maxTimeMS LANGUAGES C)
find_package(mongoc REQUIRED)
add_executable (nontailable-timeout nontailable-timeout.c)
add_executable (socketTimeoutMS socketTimeoutMS.c)

target_link_libraries(nontailable-timeout mongoc::shared)
target_link_libraries(socketTimeoutMS mongoc::shared)
