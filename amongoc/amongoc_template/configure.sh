#!/usr/bin/env bash

SCRIPT_DIR="$(dirname ${BASH_SOURCE[0]})"
VCPKG_ROOT="$HOME/vcpkg"
AMONGOC_INSTALL="TODO"

cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
      -DCMAKE_PREFIX_PATH=$AMONGOC_INSTALL \
      -Bcmake-build 
