#include "dep.h"
#include <stdlib.h>
#include <stdio.h>

int dep_fn(dep_t *d)
{
   size_t offset = (size_t)(&d->bson) - (size_t)(d);
   printf("dep has dep_t::bson at offset: %zu\n", offset);
   return d->bson.placeholder;
}
