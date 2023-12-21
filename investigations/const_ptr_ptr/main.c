void fn(const int **ppi)
{
    return;
}

typedef struct
{
    int i;
    int *pi;
} S;

// Q: How can a read-only view of S be returned without heap allocating an S?
// A:
//
// const S return_const_S(void)
// {
//     S s = {.i = 123};
//     s.pi = &s.i;
//     return s;
// }
//
// int main(void) {
//     S s = return_const_S(); // Copying struct removes const.
//     *s.pi = 123;
// }

void output_const_ptr_S(const S **out)
{
    S s = {.i = 123};
    s.pi = &s.i;
    *out = &s; // assigning to
}

int main(void)
{
    // int *pi = &i;
    // int **ppi = &pi;
    // fn(ppi); // warning: passing 'int **' to parameter of type 'const int **' discards qualifiers in nested pointer types [-Wincompatible-pointer-types-discards-qualifiers]

    int i = 123;
    // Assigning a "const pointer" to a "non-const pointer" is a warning in clang:
    const int *cpi = &i;
    int *pi;
    pi = cpi; // Results in: "warning: assigning to 'int *' from 'const int *' discards qualifiers"

    // However, assigning a "const struct with pointer" to a "non-const struct with pointer" is OK in clang:
    typedef struct
    {
        int *pi;
    } S;
    const S cs = {.pi = &i};
    S s = cs; // Not a warning.
    *s.pi = 123;
}