#include <mongoc/mongoc.h>
#include "hex.h"

int main () {
    bson_t *embedded_null_key;
    bson_t *embedded_null_json;
    bson_error_t error;
    char *str;

    mongoc_init ();

    embedded_null_key = bson_new ();
    bson_append_int32 (embedded_null_key, "a\0b", 3, 123);
    str = bin_to_hex (bson_get_data(embedded_null_key), embedded_null_key->len);
    printf ("embedded_null_key =%s\n", str);
    bson_free (str);
    bson_destroy (embedded_null_key);

    embedded_null_json = bson_new_from_json ("{\"a\\u0000b\": 123}", -1 /* length */, &error);
    if (!embedded_null_json) {
        printf ("error parsing JSON: %s", error.message);
        return EXIT_FAILURE;
    }
    str = bin_to_hex (bson_get_data(embedded_null_json), embedded_null_json->len);
    printf ("embedded_null_json=%s\n", str);
    bson_free (str);
    bson_destroy (embedded_null_json);

    mongoc_cleanup ();
}

/* Output against libmongoc 1.19.0:
 * $ ./run.sh
 * embedded_null_key =0e00000010610062007b00000000
 * embedded_null_json=0e00000010610062007b00000000
 * 
 * Parsing with bsonview shows the embedded NULL in the key:
 * echo "0e00000010610062007b00000000" | bsonview -x
 * 0e000000 10 "a" 00 62007B00 00 0000
 */ 