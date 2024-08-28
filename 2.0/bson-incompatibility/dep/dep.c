#include "dep.h"
#include <stdlib.h>
#include <stdio.h>

int dep_fn(dep_t *d)
{
   size_t offset = (size_t)(&d->bson) - (size_t)(d);
   printf("dep built with dep_t::bson offset: %zu\n", offset);
   return d->bson.placeholder;
}
