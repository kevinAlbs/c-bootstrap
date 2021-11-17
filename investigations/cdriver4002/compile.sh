clang -o workload_find workload_find.c $(PKG_CONFIG_PATH=../../install/mongo-c-driver-master/lib/pkgconfig/ pkg-config --libs --cflags libmongoc-1.0)
