cmake_minimum_required(VERSION 3.25)
project(HelloAmongoc)
add_executable(hello-amongoc hello-amongoc.c)
find_package(amongoc REQUIRED)
target_link_libraries(hello-amongoc amongoc::amongoc)
