# C Q&A

# Q4: How do you safely cast a `double` to `int64_t`?
If double > INT64_MAX, is that undefined behavior?
A:

# Q3: Why doesn't ASAN show line numbers in stack trace?
A: Note that on macOS you may need to run `dsymutil` on your binary to have the file:line info in the AddressSanitizer reports. See https://clang.llvm.org/docs/AddressSanitizer.html#symbolizing-the-reports

# Q2: What is the type of `a << b`
A: The type of `a` after integer promotion. Note, `b` may be a different integer type than `a`.

See https://github.com/mongodb/libmongocrypt/pull/454#discussion_r964225011

# Q1: Why use `inline` on static functions?
A: The inclusion of `inline` serves to silence warnings emitted by compilers given an unused static function. The unused warning is useful for functions declared in a source file (why define an unused static function?), but is misleading for those declared in a header file (may or may not be used depending on the source file including it).

See https://github.com/mongodb/libmongocrypt/pull/454#discussion_r964215352