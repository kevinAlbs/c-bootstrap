#include <bson/bson.h>
#include <mongoc/mongoc.h>

int main() {
  mongoc_init();

  // Manually construct an array (with incorrect key!):
  {
    bson_t doc = BSON_INITIALIZER, subarray;
    BSON_APPEND_ARRAY_BEGIN(&doc, "array", &subarray);
    BSON_APPEND_UTF8(&subarray, "1", "foo"); // Oops! Should be "0".
    bson_append_array_end(&doc, &subarray);

    char *as_json = bson_as_canonical_extended_json(&doc, NULL);
    printf("With BSON_APPEND_ARRAY_BEGIN: %s\n", as_json);
    bson_free(as_json);
    bson_destroy(&doc);
  }

  // Use the safer BCON API:
  {
    bson_t *doc = BCON_NEW("array", "[", "foo", "]");
    char *as_json = bson_as_canonical_extended_json(doc, NULL);
    printf("With BCON_NEW: %s\n", as_json);
    bson_free(as_json);
    bson_destroy(doc);
  }

  // Use the safer array builder:
  {
    bson_t doc = BSON_INITIALIZER;
    bson_array_builder_t *builder;
    BSON_APPEND_ARRAY_BUILDER_BEGIN(&doc, "array", &builder);
    bson_array_builder_append_utf8(builder, "foo", -1);
    bson_append_array_builder_end(&doc, builder);

    char *as_json = bson_as_canonical_extended_json(&doc, NULL);
    printf("With bson_array_builder_t: %s\n", as_json);
    bson_free(as_json);
  }

  mongoc_cleanup();
}
