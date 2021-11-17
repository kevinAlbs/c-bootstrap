export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:../../install/mongo-c-driver-master/lib/pkgconfig/"
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:../../install/libmongocrypt-master/lib/pkgconfig/"
clang -o workload_find.out workload_find.c -lpthread $(pkg-config --libs --cflags libmongoc-1.0) $(pkg-config --libs --cflags libmongocrypt)
