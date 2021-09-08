#include <mongoc/mongoc.h>

char *
bin_to_hex (const uint8_t *bin, uint32_t len)
{
   char *out = bson_malloc0 (2 * len + 1);
   uint32_t i;

   for (i = 0; i < len; i++) {
      bson_snprintf (out + (2 * i), 3, "%02x", bin[i]);
   }
   return out;
}

int main () {
   bson_t *bson;
   char* hex;
   char *legacy_extended_json;
   char *canonical_extended_json;
   char *relaxed_extended_json;

   mongoc_init ();

   bson = bson_new ();
   BSON_APPEND_BINARY (bson, "foo", 7 /* hard coded */, (uint8_t*) "\0", 1);
   
   hex = bin_to_hex (bson_get_data (bson), bson->len);
   legacy_extended_json = bson_as_json (bson, NULL);
   canonical_extended_json = bson_as_canonical_extended_json (bson, NULL);
   relaxed_extended_json = bson_as_canonical_extended_json (bson, NULL);

   printf ("hex: %s\n", hex);
   printf ("legacy extended JSON: %s\n", legacy_extended_json);
   printf ("relaxed extended JSON: %s\n", relaxed_extended_json);
   printf ("canonical extended JSON: %s\n", canonical_extended_json);

   bson_free (relaxed_extended_json);
   bson_free (canonical_extended_json);
   bson_free (legacy_extended_json);
   bson_free (hex);
   bson_destroy (bson);   

   mongoc_cleanup ();
}