set -o errexit
set -o xtrace

GITREF=callcount.0eeff0a44
rm -rf cmake-build-$GITREF
cmake \
    -DCMAKE_PREFIX_PATH="$HOME/mongo-c-driver-$GITREF" \
    -DCMAKE_BUILD_TYPE="RelWithDebInfo" \
    -S./ \
    -B./cmake-build-$GITREF
cmake --build cmake-build-$GITREF --target run-workload

GITREF=callcount.ded9ae5e9
rm -rf cmake-build-$GITREF
cmake \
    -DCMAKE_PREFIX_PATH="$HOME/mongo-c-driver-$GITREF" \
    -DCMAKE_BUILD_TYPE="RelWithDebInfo" \
    -S./ \
    -B./cmake-build-$GITREF
cmake --build cmake-build-$GITREF --target run-workload

GITREF=callcount.1.29.0
rm -rf cmake-build-$GITREF
cmake \
    -DCMAKE_PREFIX_PATH="$HOME/mongo-c-driver-$GITREF" \
    -DCMAKE_BUILD_TYPE="RelWithDebInfo" \
    -S./ \
    -B./cmake-build-$GITREF
cmake --build cmake-build-$GITREF --target run-workload
