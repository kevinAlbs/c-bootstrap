export PKG_CONFIG_PATH=/Users/kevin.albertson/install/mongo-c-driver/lib/pkgconfig
clang -o subtype7.out main.c $(pkg-config --libs --cflags libmongoc-1.0)