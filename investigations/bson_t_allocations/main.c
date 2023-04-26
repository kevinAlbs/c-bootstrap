// A common pattern in the C driver and libmongocrypt is to expect an uninitialized `bson_t` as an output parameter.
// This prevents callers from heap allocating `bson_t`.

#include <mongoc/mongoc.h>

void callee_initializes(bson_t *out)
{
    bson_init(out);
    BSON_APPEND_UTF8(out, "foo", "bar");
}

void caller_initializes(bson_t *out)
{
    BSON_APPEND_UTF8(out, "foo", "bar");
}

int main()
{
    mongoc_init();

    // If callee initializes, passing a heap allocated bson_t results in leak.
    {
        bson_t *heap_allocated = bson_new();

        callee_initializes(heap_allocated); // Reinitializes the bson_t as stack allocated.
        bson_destroy(heap_allocated);       // Does not destroy bson_t struct!
    }

    // If caller initializes, passing a heap allocated bson_t is OK.
    {
        bson_t *heap_allocated = bson_new();

        caller_initializes(heap_allocated);
        bson_destroy(heap_allocated); // Destroys the bson_t struct.
    }

    mongoc_cleanup();
}
