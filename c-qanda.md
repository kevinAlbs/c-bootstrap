# C Q&A

# Q2: What is the type of `a << b`
A: The type of `a` after integer promotion. Note, `b` may be a different integer type than `a`.

See https://github.com/mongodb/libmongocrypt/pull/454#discussion_r964225011

# Q1: Why use `inline` on static functions?
A: The inclusion of `inline` serves to silence warnings emitted by compilers given an unused static function. The unused warning is useful for functions declared in a source file (why define an unused static function?), but is misleading for those declared in a header file (may or may not be used depending on the source file including it).

See https://github.com/mongodb/libmongocrypt/pull/454#discussion_r964215352