cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(generator_expressions LANGUAGES C)
find_package(mongoc-1.0 REQUIRED)
add_executable (main.out main.c)
target_link_libraries(main.out mongo::mongoc_static)

# Suggested technique for debugging generator expressions:
# https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html#debugging
add_custom_target (genexdebug COMMAND ${CMAKE_COMMAND} -E echo "$<UPPER_CASE:AppleClang>")
