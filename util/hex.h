#ifndef C_BOOTSTRAP_UTIL_HEX
#define C_BOOTSTRAP_UTIL_HEX

static BSON_INLINE char *
bin_to_hex (const uint8_t *bin, uint32_t len)
{
   char *out = bson_malloc0 (2 * len + 1);
   uint32_t i;

   for (i = 0; i < len; i++) {
      bson_snprintf (out + (2 * i), 3, "%02x", bin[i]);
   }
   return out;
}

#endif /* C_BOOTSTRAP_UTIL_HEX */