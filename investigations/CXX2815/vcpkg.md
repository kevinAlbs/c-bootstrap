```bash
./vcpkg depend-info mongo-cxx-driver --format=tree
```

Produces:
```
+-- libbson
|   +-- vcpkg-cmake
|   +-- vcpkg-cmake-config
+-- mongo-c-driver
|   +-- libbson
|   |   +- ...
|   +-- vcpkg-cmake
|   +-- vcpkg-cmake-config
|   +-- zlib
|       +-- vcpkg-cmake
+-- vcpkg-cmake
+-- vcpkg-cmake-config
```
