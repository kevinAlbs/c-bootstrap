set -o errexit

GITREF="ded9ae5e9-callcount"
cmake \
    -DCMAKE_PREFIX_PATH="$HOME/mongo-c-driver-$GITREF" \
    -S./ \
    -B./cmake-build-$GITREF
cmake --build "cmake-build-$GITREF" --target all

GITREF="fastcopy"
cmake \
    -DCMAKE_PREFIX_PATH="$HOME/mongo-c-driver-$GITREF" \
    -S./ \
    -B./cmake-build-$GITREF
cmake --build "cmake-build-$GITREF" --target all
