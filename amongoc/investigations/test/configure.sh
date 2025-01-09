#!/usr/bin/env bash

SCRIPT_DIR="$(dirname ${BASH_SOURCE[0]})"
VCPKG_ROOT="$HOME/vcpkg"
AMONGOC_INSTALL="/home/admin/mongo-c-driver-async/mongo-c-driver-async-0.1.0-async-pr4"

cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
      -DCMAKE_PREFIX_PATH=$AMONGOC_INSTALL \
      -Bcmake-build 
