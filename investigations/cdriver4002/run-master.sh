export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:../../install/mongo-c-driver-master/lib/"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:../../install/libmongocrypt-master/lib/"
export DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:../../install/mongo-c-driver-master/lib/"
export DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:../../install/libmongocrypt-master/lib/"

./workload_find.out $@
