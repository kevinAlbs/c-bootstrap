#ifndef UTIL_ASSERTIONS
#define UTIL_ASSERTIONS

#define ASSERT BSON_ASSERT
#define ASSERTF(stmt, ...)                                                           \
    if (!(stmt))                                                                     \
    {                                                                                \
        fprintf(stderr, "error at statement \"%s\" on line: %d\n", #stmt, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                                                \
        fprintf(stderr, "\n");                                                       \
        abort();                                                                     \
    }                                                                                \
    else                                                                             \
        (void)0

#endif // UTIL_ASSERTIONS
