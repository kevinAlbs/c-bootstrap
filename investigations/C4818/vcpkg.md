```bash
./vcpkg depend-info mongo-c-driver --format=tree
```

Produces:
```
mongo-c-driver
+-- libbson
|   +-- vcpkg-cmake
|   +-- vcpkg-cmake-config
+-- vcpkg-cmake
+-- vcpkg-cmake-config
+-- zlib
    +-- vcpkg-cmake
```
