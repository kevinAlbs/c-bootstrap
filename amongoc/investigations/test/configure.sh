#!/usr/bin/env bash

SCRIPT_DIR="$(dirname ${BASH_SOURCE[0]})"
VCPKG_ROOT="$HOME/vcpkg"
AMONGOC_INSTALL="$HOME/amongoc-0.1.0"

# For macOS, specify a newer clang:
# Use clang 19.
export CC=/opt/homebrew/opt/llvm/bin/clang
export CXX=/opt/homebrew/opt/llvm/bin/clang++

cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
      -DCMAKE_PREFIX_PATH=$AMONGOC_INSTALL \
      -DCMAKE_C_STANDARD=23 \
      -Bcmake-build 
