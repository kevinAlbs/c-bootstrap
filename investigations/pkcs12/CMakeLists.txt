cmake_minimum_required(VERSION 3.15)
project(main C)
add_executable(main main.c)
target_link_libraries(main ncrypt crypt32)