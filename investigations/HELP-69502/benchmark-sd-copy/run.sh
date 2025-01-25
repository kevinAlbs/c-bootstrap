set -o errexit

GITREF="ded9ae5e9-callcount"
echo "Testing $GITREF"
./cmake-build-$GITREF/benchmark-sd-copy

GITREF="fastcopy"
echo "Testing $GITREF"
./cmake-build-$GITREF/benchmark-sd-copy
