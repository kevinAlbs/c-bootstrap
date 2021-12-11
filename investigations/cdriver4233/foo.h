/* Q: Why is foo the function not detected by check_symbol_exists?
 * A: I think because CheckSymbolExists tries compiling a test program.
*/
static inline int foo() {
    return 0;
}

// #define foo 1
