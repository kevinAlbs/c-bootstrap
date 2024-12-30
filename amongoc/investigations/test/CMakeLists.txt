cmake_minimum_required(VERSION 3.25)
project(HelloAmongoc)
add_executable(hello-amongoc hello-amongoc.c)
find_package(amongoc REQUIRED)
target_link_libraries(hello-amongoc PRIVATE amongoc::amongoc)
# TODO: work around linker error:
target_link_libraries(hello-amongoc PRIVATE /home/admin/review/mongo-c-driver-async-2/_build/vcpkg_installed/x64-linux/lib/libneo-fun.a)
