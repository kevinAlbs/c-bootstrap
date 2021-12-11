#define XSTR(x) STR(x)
#define STR(x) #x
#pragma message "The value of _NONSTD_SOURCE: " XSTR(_NONSTD_SOURCE)
#pragma message "The value of _POSIX_C_SOURCE: " XSTR(_POSIX_C_SOURCE)
#pragma message "The value of __DARWIN_C_LEVEL: " XSTR(__DARWIN_C_LEVEL)
#pragma message "The value of __DARWIN_C_FULL: " XSTR(__DARWIN_C_FULL)
#pragma message "The value of _DARWIN_C_SOURCE: " XSTR(_DARWIN_C_SOURCE)

#include "foo.h"
#include <string.h>
#include <stdio.h>

int main () {
    char dst[6];
    strlcpy (dst, "hello", 5);
    printf ("copied: %s", dst);
}
