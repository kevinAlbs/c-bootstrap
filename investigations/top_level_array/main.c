#include <mongoc/mongoc.h>

int main () {
    mongoc_init ();

    char * json = "[0,1,2]";
    bson_t b = BSON_INITIALIZER;
    bson_error_t error = {0};
    bson_init_from_json(&b, json, -1, &error);
    char * out = bson_as_canonical_extended_json(&b, NULL);
    printf("%s\n", out);

    mongoc_cleanup ();
}
