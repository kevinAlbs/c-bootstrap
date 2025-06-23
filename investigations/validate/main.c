#include <stdio.h>
#include "bson/bson.h"

const char bad_bson[] = "\223\2\0\0\4databases\0\322\1\0\0\0030\0001\0\0\0\2name\0\6\0\0\0admin\0\22sizeOnDisk\0\0`\5\0\0\0\0\0\bempty\0\0\0\0031\0002\0\0\0\2name\0\a\0\0\0config\0\22sizeOnDisk\0\0\240\5\0\0\0\0\0\bempty\0\0\0\0032\0009\0\0\0\2name\0\16\0\0\0dbFromAtlasUI\0\22sizeOnDisk\0\0 \0\0\0\0\0\0\bempty\0\0\0\0033\0002\0\0\0\2name\0\a\0\0\0dbtest\0\22sizeOnDisk\0\0 \1\0\0\0\0\0\bempty\0\0\0\0034\0001\0\0\0\2name\0\6\0\0\0local\0\22sizeOnDisk\0\0\260\320\0\0\0\0\0\bempty\0\0\0\0035\0O\0\0\0\2name\0$\0\0\0mongosync_reserved_for_internal_use\0\22sizeOnDisk\0\0\0\b\0\0\0\0\0\bempty\0\0\0\0036\0002\0\0\0\2name\0\a\0\0\0somedb\0\22sizeOnDisk\0\0 \0\0\0\0\0\0\bempty\0\0\0\0037\0005\0\0\0\2name\0\n\0\0\0"

// A non-UTF-8 key:
// "weird\377\377db"
"weirdffdb"

// … the rest of the document:
"\0\22sizeOnDisk\0\0 \0\0\0\0\0\0\bempty\0\0\0\0\22totalSize\0\0000\345\0\0\0\0\0\22totalSizeMb\0\16\0\0\0\0\0\0\0\1ok\0\0\0\0\0\0\0\360?\3$clusterTime\0X\0\0\0\21clusterTime\0\1\0\0\0*uIh\3signature\0003\0\0\0\5hash\0\24\0\0\0\0\332\373\27\205\13<}?\201\314~\260\252\341\332)\261\300\363v\22keyId\0\5\0\0\0\271\rCh\0\0\21operationTime\0\1\0\0\0*uIh\0";

int main() {
    printf ("libbson=%s\n", bson_get_version());
    printf("hello (size of bson=%lu)\n", sizeof(bad_bson));

    bson_t* parsed = bson_new_from_data(
        (const uint8_t*) bad_bson,
        sizeof(bad_bson) - 1
    );

    if (parsed == NULL) {
        printf("failed bson_new");
        return 2;
    }

    printf ("as_json: %s", bson_as_canonical_extended_json (parsed, NULL));

    bson_error_t err;
    bool is_valid = bson_validate_with_error(
        parsed,
        BSON_VALIDATE_UTF8 | BSON_VALIDATE_UTF8_ALLOW_NULL,
        &err
    );

    if (is_valid) {
        printf("fails to find invalid UTF-8\n");
        return 1;
    }

    return 0;
}
