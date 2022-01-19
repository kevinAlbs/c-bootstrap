#ifndef C_BOOTSTRAP_UTIL_HEX
#define C_BOOTSTRAP_UTIL_HEX

#include <ctype.h> /* tolower */

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

static BSON_INLINE int
unhexlify (const char *in, size_t len)
{
   int i;
   int byte;
   int total = 0;
   int multiplier = 1;

   for (i = (int) len - 1; i >= 0; i--) {
      char c = *(in + i);

      if (c >= '0' && c <= '9') {
         byte = c - 48;
      } else if (c >= 'a' && c <= 'f') {
         byte = c - 97 + 10;
      } else if (c >= 'A' && c <= 'F') {
         byte = c - 65 + 10;
      } else {
         return -1;
      }

      total += byte * multiplier;
      multiplier *= 16;
   }
   return total;
}

/* filters out '|', ' ', ',', and "0x" */
static BSON_INLINE char *
copy_and_filter_hex (const char *unfiltered_hex)
{
   size_t i, j;

   char *filtered = malloc (strlen (unfiltered_hex) + 1);
   j = 0;
   for (i = 0; i < strlen (unfiltered_hex); i++) {
      if (unfiltered_hex[i] == ' ') {
         continue;
      }
      if (unfiltered_hex[i] == '|') {
         continue;
      }
      if (unfiltered_hex[i] == ',') {
         continue;
      }
      if (i < strlen(unfiltered_hex) - 1 && 0 == strncmp (unfiltered_hex + i, "0x", 2)) {
         i++;
         continue;
      }

      filtered[j] = (char) tolower (unfiltered_hex[i]);
      j++;
   }
   filtered[j] = '\0';
   return filtered;
}

static BSON_INLINE uint8_t *
hex_to_bin (char *unfiltered_hex, size_t *outlen)
{
   char *filtered_hex;
   uint8_t *bytes;
   size_t i;

   filtered_hex = copy_and_filter_hex (unfiltered_hex);
   *outlen = strlen (filtered_hex) / 2;
   bytes = malloc (*outlen);
   for (i = 0; i < *outlen; i++) {
      bytes[i] = unhexlify (filtered_hex + (i * 2), 2);
   }

   free (filtered_hex);
   return bytes;
}

#endif /* C_BOOTSTRAP_UTIL_HEX */