cmake_minimum_required(VERSION 3.12)

project(dllexport VERSION 1.0.0 LANGUAGES C)

# Add a library with a variety of export symbols.
add_library(lib1_shared SHARED lib1.c)
add_library(lib1_static STATIC lib1.c)

# `lib2` consumes `lib1`
# `lib2` intends to hide symbols of `lib1`.
add_library(lib2_shared_lib1_shared SHARED lib2.c)
target_compile_definitions(lib2_shared_lib1_shared PRIVATE LIB2_SHARED)
target_link_libraries(lib2_shared_lib1_shared PRIVATE lib1_shared)

add_library(lib2_shared_lib1_static SHARED lib2.c)
target_compile_definitions(lib2_shared_lib1_shared PRIVATE LIB2_SHARED)
target_link_libraries(lib2_shared_lib1_static PRIVATE lib1_static)

add_library(lib2_static_lib1_shared STATIC lib2.c)
target_link_libraries(lib2_static_lib1_shared PRIVATE lib1_shared)

add_library(lib2_static_lib1_static STATIC lib2.c)
target_link_libraries(lib2_static_lib1_static PRIVATE lib1_static)