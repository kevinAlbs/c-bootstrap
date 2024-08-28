#ifndef BSON_H
#define BSON_H

// `bson_t` is aligned at 128 bytes.
typedef struct
{
    int placeholder;
} bson_t __attribute__((aligned(128)));

#endif
