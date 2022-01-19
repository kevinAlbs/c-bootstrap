Here is the full output of the `cmake --build . --target main.out` command:

```
/usr/local/Cellar/cmake/3.22.0/bin/cmake -S/Users/kevin.albertson/code/c-bootstrap/investigations/can_i_build_with_other_macos_sdk -B/Users/kevin.albertson/code/c-bootstrap/investigations/can_i_build_with_other_macos_sdk/cmake-build --check-build-system CMakeFiles/Makefile.cmake 0
/Applications/Xcode.app/Contents/Developer/usr/bin/make  -f CMakeFiles/Makefile2 main.out
/usr/local/Cellar/cmake/3.22.0/bin/cmake -S/Users/kevin.albertson/code/c-bootstrap/investigations/can_i_build_with_other_macos_sdk -B/Users/kevin.albertson/code/c-bootstrap/investigations/can_i_build_with_other_macos_sdk/cmake-build --check-build-system CMakeFiles/Makefile.cmake 0
/usr/local/Cellar/cmake/3.22.0/bin/cmake -E cmake_progress_start /Users/kevin.albertson/code/c-bootstrap/investigations/can_i_build_with_other_macos_sdk/cmake-build/CMakeFiles 2
/Applications/Xcode.app/Contents/Developer/usr/bin/make  -f CMakeFiles/Makefile2 CMakeFiles/main.out.dir/all
/Applications/Xcode.app/Contents/Developer/usr/bin/make  -f CMakeFiles/main.out.dir/build.make CMakeFiles/main.out.dir/depend
cd /Users/kevin.albertson/code/c-bootstrap/investigations/can_i_build_with_other_macos_sdk/cmake-build && /usr/local/Cellar/cmake/3.22.0/bin/cmake -E cmake_depends "Unix Makefiles" /Users/kevin.albertson/code/c-bootstrap/investigations/can_i_build_with_other_macos_sdk /Users/kevin.albertson/code/c-bootstrap/investigations/can_i_build_with_other_macos_sdk /Users/kevin.albertson/code/c-bootstrap/investigations/can_i_build_with_other_macos_sdk/cmake-build /Users/kevin.albertson/code/c-bootstrap/investigations/can_i_build_with_other_macos_sdk/cmake-build /Users/kevin.albertson/code/c-bootstrap/investigations/can_i_build_with_other_macos_sdk/cmake-build/CMakeFiles/main.out.dir/DependInfo.cmake --color=
/Applications/Xcode.app/Contents/Developer/usr/bin/make  -f CMakeFiles/main.out.dir/build.make CMakeFiles/main.out.dir/build
[ 50%] Building C object CMakeFiles/main.out.dir/main.c.o
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc   -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX12.0.sdk -mmacosx-version-min=11.6 -MD -MT CMakeFiles/main.out.dir/main.c.o -MF CMakeFiles/main.out.dir/main.c.o.d -o CMakeFiles/main.out.dir/main.c.o -c /Users/kevin.albertson/code/c-bootstrap/investigations/can_i_build_with_other_macos_sdk/main.c
[100%] Linking C executable main.out
/usr/local/Cellar/cmake/3.22.0/bin/cmake -E cmake_link_script CMakeFiles/main.out.dir/link.txt --verbose=1
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX12.0.sdk -mmacosx-version-min=11.6 -Wl,-search_paths_first -Wl,-headerpad_max_install_names CMakeFiles/main.out.dir/main.c.o -o main.out 
[100%] Built target main.out
/usr/local/Cellar/cmake/3.22.0/bin/cmake -E cmake_progress_start /Users/kevin.albertson/code/c-bootstrap/investigations/can_i_build_with_other_macos_sdk/cmake-build/CMakeFiles 0
```

Here is an annotation of the build of `main.c`:

```bash
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc \
    # -isysroot <dir>         Set the system root directory (usually /)
    -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX12.0.sdk \
    # -mmacosx-version-min=<value>
    #     Set Mac OS X deployment target
    -mmacosx-version-min=11.6 \
    # -MD                     Write a depfile containing user and system headers
    -MD \
    # -MT <value>             Specify name of main file output in depfile
    -MT \
    CMakeFiles/main.out.dir/main.c.o \
    # -MF <file>              Write depfile output from -MMD, -MD, -MM, or -M to <file>
    -MF \
    CMakeFiles/main.out.dir/main.c.o.d \
    -o CMakeFiles/main.out.dir/main.c.o \
    -c /Users/kevin.albertson/code/c-bootstrap/investigations/can_i_build_with_other_macos_sdk/main.c
```