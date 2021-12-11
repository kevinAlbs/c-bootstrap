cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(cdriver4233 LANGUAGES C)

include(CheckIncludeFile)
include(CheckSymbolExists)

# check_include_file (/Users/kevin.albertson/code/c-bootstrap/investigations/cdriver4233/foo.h HAS_FOO_H)
# message ("HAS_FOO_H? ${HAS_FOO_H}")


# check_symbol_exists (foo /Users/kevin.albertson/code/c-bootstrap/investigations/cdriver4233/foo.h HAS_FOO)
# message ("HAS_FOO? ${HAS_FOO}")

check_symbol_exists (strlcpy string.h HAS_STRLCPY)
message ("HAS_STRLCPY? ${HAS_STRLCPY}")

add_executable (main.out main.c)
target_link_libraries(main.out)
