cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(ctest LANGUAGES C)
add_executable(test.out test_foobar.c test.c foobar.c)
enable_testing ()
add_test (test_bar test.out bar)
add_test (test_foo test.out foo)

