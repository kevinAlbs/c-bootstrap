cmake_minimum_required (VERSION 3.25)
project (hello-mongoac LANGUAGES C CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Point CMake at the install tree, e.g.:
#   cmake -S. -Bbuild -DCMAKE_PREFIX_PATH=/absolute/path/to/mongoac-spec/.install

# bson's exported package works normally.
find_package (bson 2.3.1 REQUIRED)

# db-queue drives the runtime on a worker thread.
find_package (Threads REQUIRED)

# TODO: use find_package(mongoac).
find_path (MONGOAC_INCLUDE_DIR mongoac/client.h
  PATH_SUFFIXES include/mongoac-0.1.0 mongoac-0.1.0
  REQUIRED
)
find_library (MONGOAC_LIBRARY mongoac0 REQUIRED)

add_executable (db-queue db-queue.cpp)
target_include_directories (db-queue PRIVATE "${MONGOAC_INCLUDE_DIR}")
target_link_libraries (db-queue PRIVATE "${MONGOAC_LIBRARY}" bson::shared Threads::Threads)
