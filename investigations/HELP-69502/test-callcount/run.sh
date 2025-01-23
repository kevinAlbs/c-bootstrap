: "${MONGODB_URI:?"Set to the URI to connect"}"

export DYLD_LIBRARY_PATH="$HOME/code/tasks/mongo-c-driver-HELP-69502/.install-0eeff0a44-callcount/lib"
echo "Testing DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH"
time ./cmake-build/run-workload

export DYLD_LIBRARY_PATH="$HOME/code/tasks/mongo-c-driver-HELP-69502/.install-ded9ae5e9-callcount/lib"
echo "Testing DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH"
time ./cmake-build/run-workload

export DYLD_LIBRARY_PATH="$HOME/code/tasks/mongo-c-driver-HELP-69502/.install-1.29.0-callcount/lib"
echo "Testing DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH"
time ./cmake-build/run-workload
