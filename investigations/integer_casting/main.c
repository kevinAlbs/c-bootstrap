#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

int main () {
    // Q: What is the maximum and minimum value of an int8 and uint8?
    printf ("INT8_MIN = %" PRId8 ", INT8_MAX=%" PRId8 "\n", (int8_t) INT8_MIN, (int8_t) INT8_MAX);
    printf ("UINT8_MIN = %" PRIu8 ", UINT8_MAX=%" PRIu8 "\n", (uint8_t) 0, (uint8_t) UINT8_MAX);
    // A: [-2^7, 2^7 - 1], [0, 2^8 - 1]

    // Q: What happens when signed => unsigned?
    int8_t i8 = -1;
    uint8_t u8 = (uint8_t) i8;
    printf ("-1 converted to uint8=%" PRIu8 "\n", u8);
    // A: It wraps. Add 1+UNSIGNED MAX.
    // See C89 standard:
    // https://port70.net/~nsz/c/c89/c89-draft.html#3.2.1.2

    // Q: What if the unsigned is smaller?
    int32_t i32 = -512 * 2;
    u8 = (uint8_t) i32;
    printf ("-512 converted to uint8=%" PRIu8 "\n", u8);
    // A: It divides modulo 1+UNSIGNED MAX.

    // Q: What about signed => smaller signed? Or unsigned OOB => signed?
    u8 = 255;
    i8 = (int8_t) u8;
    printf ("255 converted to int8=%" PRId8 "\n", i8);
    // A: Implementation defined.
}
