#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // strtod
#include <string.h> // memcmp

int main()
{
    char str20[32];
    char str16[32];

    snprintf(str20, sizeof(str20), "%.20g", 1.99);
    snprintf(str16, sizeof(str16), "%.16g", 1.99);

    printf("1.99 represented with 20 digits of precision is: %s\n", str20); // 1.9899999999999999911
    printf("1.99 represented with 16 digits of precision is: %s\n", str16); // 1.99

    char *endptr;
    double d20 = strtod(str20, &endptr);
    assert(*endptr == '\0'); // Assert entire string was parsed.
    double d16 = strtod(str16, &endptr);
    assert(*endptr == '\0'); // Assert entire string was parsed.
    assert(d20 == d16);

    // Check that both doubles have the same memory representation.
    assert(0 == memcmp(&d20, &d16, sizeof(double)));
}
