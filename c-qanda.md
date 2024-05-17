# C Q&A

# Q8: Are fields not referenced in designated initializers zero-initialized?
Example:

```c
typedef struct {
    int bar;
    int baz;
} foo;

foo f = { .bar = 123 }; // Is `baz` guaranteed to be 0?
```

A: Yes. See https://stackoverflow.com/a/3374468/774658


A: (Open)

# Q7: Why is assigning a "const struct with pointer" to a "non-const struct with pointer" not a warning?

```c
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
```
A:

# Q6: Why is passing a "non-const pointer pointer" to a "const pointer pointer" a warning?

```c
void fn(const int **ppi)
{
    return;
}

int main(void)
{
    int i = 123;
    int *pi = &i;
    int **ppi = &pi;
    fn(ppi); // warning: passing 'int **' to parameter of type 'const int **' discards qualifiers in nested pointer types [-Wincompatible-pointer-types-discards-qualifiers]
}
```

A:

# Q5: Is changing a pointed value in a `const` struct permitted?

This test suggests "yes":

```c
typedef struct {
    int *pi;
} foo;

int i = 0, j = 0;
const foo cf = {.pi = &i};
// cf.pi = &j;                   // Error: 'cannot assign'.
*cf.pi = 123;                    // Q: Why is this not a compiler error? A:
ASSERT(*cf.pi == 123 && j == 0); // Silence unused warning.
```

A:

I expect `const` is applied to each field in `foo` as follows:

```c
// `const foo` is equivalent to:
typedef struct {
    int * const pi; // `pi` cannot be changed. `*pi` can be changed.
} const_foo
```


# Q4: How do you safely cast a `double` to `int64_t`?
If double > INT64_MAX, is that undefined behavior?
A:

# Q3: Why doesn't ASAN show line numbers in stack trace?
A: Note that on macOS you may need to run `dsymutil` on your binary to have the file:line info in the AddressSanitizer reports. See https://clang.llvm.org/docs/AddressSanitizer.html#symbolizing-the-reports
Try: `dsymutil ./cmake-build/src/libmongoc/test-libmongoc`

# Q2: What is the type of `a << b`
A: The type of `a` after integer promotion. Note, `b` may be a different integer type than `a`.

See https://github.com/mongodb/libmongocrypt/pull/454#discussion_r964225011

# Q1: Why use `inline` on static functions?
A: The inclusion of `inline` serves to silence warnings emitted by compilers given an unused static function. The unused warning is useful for functions declared in a source file (why define an unused static function?), but is misleading for those declared in a header file (may or may not be used depending on the source file including it).

See https://github.com/mongodb/libmongocrypt/pull/454#discussion_r964215352
