#include "bson2.h" // Use default-aligned `bson_t`.
#include "dep.h"

#include <stdio.h>

int main(void)
{
   dep_t d = {0};
   d.bson.placeholder = 123;

   size_t offset = (size_t)(&d.bson) - (size_t)(&d);
   printf("app has dep_t::bson at offset: %zu\n", offset);
   int got = dep_fn(&d);
   if (got != 123)
   {
      printf("Error: expected 123, but got: %d\n", got);
   }
   return 0;
}
