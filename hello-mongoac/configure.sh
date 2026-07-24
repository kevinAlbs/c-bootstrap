#!/usr/bin/env bash
# Configure the hello-mongoac project with CMake.
set -eu

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${here}/cmake-build"
prefix="/Users/kevin.albertson/code/mongoac-spec/.install"

cmake \
  -S "${here}" \
  -B "${build_dir}" \
  -DCMAKE_PREFIX_PATH="${prefix}" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Make compile_commands.json discoverable from the project root.
ln -sf cmake-build/compile_commands.json "${here}/compile_commands.json"

echo
echo "Configured. Build with: cmake --build ${build_dir}"
