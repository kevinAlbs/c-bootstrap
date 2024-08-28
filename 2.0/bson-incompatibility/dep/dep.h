#include "bson1.h"

typedef struct
{
   int placeholder;
   bson_t bson;
} dep_t;

int dep_fn(dep_t *d);
