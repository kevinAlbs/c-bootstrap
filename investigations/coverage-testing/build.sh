# cmake --build cmake-build --target main.out

/opt/homebrew/opt/llvm/bin/clang -O0 -o main.out -fprofile-instr-generate -fcoverage-mapping main.c 
