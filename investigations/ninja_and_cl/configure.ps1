# Run with Developer Powershell for Visual Studio:
cmake `
    -G "Ninja" `
    -D CMAKE_C_COMPILER="cl.exe" `
    -S . `
    -B cmake-build