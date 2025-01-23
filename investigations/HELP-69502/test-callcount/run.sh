: "${MONGODB_URI:?"Set to the URI to connect"}"

GITREF=callcount.0eeff0a44
echo "Testing $GITREF"
time ./cmake-build-$GITREF/run-workload

GITREF=callcount.ded9ae5e9
echo "Testing $GITREF"
time ./cmake-build-$GITREF/run-workload

GITREF=callcount.1.29.0
echo "Testing $GITREF"
time ./cmake-build-$GITREF/run-workload
