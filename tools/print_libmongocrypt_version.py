#!/usr/bin/env python3
"""
print_libmongocrypt_version.py prints a version of libmongocrypt
"""
import argparse
import bson
import bson.binary
from bson import json_util
import os
import sys
try:
    import cffi
except ModuleNotFoundError as e:
    print(e)
    print("Try installing 'cffi' with 'pip install cffi'")
    sys.exit(1)

ffi = cffi.FFI()
# Use preprocess_header.py to generate the cdef string.
ffi.cdef("""

const char *
mongocrypt_version (uint32_t *len);
"""
         )

# lib is the returned object from ffi.dlopen.
_lib = None


class _lib_Wrapper:
    def __init__(self, libpath: str):
        global _lib
        _lib = ffi.dlopen(libpath)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        ffi.dlclose(_lib)


def get_version(libpath: str):
    with _lib_Wrapper(libpath):
        version_cdata = _lib.mongocrypt_version(ffi.NULL)
        version = ffi.string(version_cdata)
        return version.decode("utf8")


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--version", action="store_true",
                        help="Print version of libmongocrypt")
    parser.add_argument(
        "--libpath", help="Path to the crypt shared library. May also be set through CRYPT_SHARED_PATH environment variable")
    parser.add_argument(
        "--inpath", help="Path to a file containing the command to mark up. The format must be extended canonical JSON. If not present, input is read from stdin.")
    args = parser.parse_args()
    libpath = None
    if args.libpath:
        libpath = args.libpath
    elif "LIBMONGOCRYPT_PATH" in os.environ:
        libpath = os.environ["LIBMONGOCRYPT_PATH"]
    else:
        raise argparse.ArgumentError(None,
                                     "Must specify --libpath argument or LIBMONGOCRYPT_PATH argument")

    if args.version:
        print(get_version(libpath))
        return


if __name__ == "__main__":
    main()
