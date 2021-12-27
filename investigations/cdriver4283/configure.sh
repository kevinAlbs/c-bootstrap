cmake \
    -DCMAKE_PREFIX_PATH=../../install/mongo-c-driver-1.19.0 \
    -DCMAKE_BUILD_TYPE="Debug" \
    -S./ \
    -B./cmake-build
