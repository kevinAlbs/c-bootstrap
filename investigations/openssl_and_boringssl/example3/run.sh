# Q: What does -fvisibility=hidden do?
# A: Sets the default symbol visibility for all global declarations.

# Add -fvisibility=hidden to libfunc.dylib build to observe linker error.
clang -o libfunc.dylib -shared func.c
clang -o app.out app.c -L. -lfunc