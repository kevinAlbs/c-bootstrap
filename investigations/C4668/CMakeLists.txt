cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(C4668 LANGUAGES C)

find_package(PkgConfig)
pkg_check_modules(PC_Utf8proc libutf8proc)

find_path(Utf8proc_INCLUDE_DIR
    NAMES utf8proc.h
    PATHS ${PC_utf8proc_INCLUDE_DIRS}
    PATH_SUFFIXES utf8proc-2.8.0
)

set(Utf8proc_VERSION ${PC_utf8proc_VERSION})

mark_as_advanced(Utf8proc_FOUND Utf8proc_INCLUDE_DIR Utf8proc_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Utf8proc
    REQUIRED_VARS Utf8proc_INCLUDE_DIR
    VERSION_VAR Utf8proc_VERSION
)

# TODO: define a utf8proc imported target, and do: `link_library (main.out utf8proc)`

add_executable (main.out main.c)
