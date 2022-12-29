#include <assert.h>
#include <stdio.h>  // printf
#include <stdlib.h> // malloc
#include <stdint.h> // uint8_t
#include <string.h> // memcpy

typedef struct _mongoc_array_t mongoc_array_t;

struct _mongoc_array_t
{
    size_t len;
    size_t element_size;
    size_t allocated;
    void *data;
};

#define _mongoc_array_append_val(a, v) _mongoc_array_append_vals(a, &v, 1)
#define _mongoc_array_index(a, t, i) (((t *)(a)->data)[i])
#define _mongoc_array_clear(a) (a)->len = 0

void _mongoc_array_init(mongoc_array_t *array, size_t element_size)
{
    assert(array);
    assert(element_size > 0);

    array->len = 0;
    array->element_size = element_size;
    array->allocated = 128;
    array->data = (void *)calloc(1, array->allocated);
}

void _mongoc_array_destroy(mongoc_array_t *array)
{
    if (array && array->data)
    {
        free(array->data);
    }
}

// next_power_of_two returns the next power of two greater than `in`.
static size_t next_power_of_two(size_t in)
{
    size_t p2 = 1;
    while (p2 <= in)
    {
        p2 *= 2;
    }
    return p2;
}

void _mongoc_array_append_vals(mongoc_array_t *array,
                               const void *data,
                               uint32_t n_elements)
{
    size_t len;
    size_t off;
    size_t next_size;

    assert(array);
    assert(data);

    off = array->element_size * array->len;
    len = (size_t)n_elements * array->element_size;
    if ((off + len) > array->allocated)
    {
        next_size = next_power_of_two(off + len);
        array->data = realloc(array->data, next_size);
        array->allocated = next_size;
    }

    memcpy((uint8_t *)array->data + off, data, len);

    array->len += n_elements;
}

struct mystruct
{
    int x;
    int y;
};

typedef int32_t i32alias;

int main()
{

    // Test with an array of int32_t.
    mongoc_array_t arr_i32;
    {
        _mongoc_array_init(&arr_i32, sizeof(int32_t));

        // Append the values [1,2].
        int32_t i32;
        i32 = 1;
        _mongoc_array_append_val(&arr_i32, i32);
        i32 = 2;
        _mongoc_array_append_val(&arr_i32, i32);

        // Assert values are in array.
        int32_t got;
        got = _mongoc_array_index(&arr_i32, int32_t, 0);
        assert(got == 1);
        got = _mongoc_array_index(&arr_i32, int32_t, 1);
        assert(got == 2);
    }

    // Test with an array of structs.
    mongoc_array_t arr_mystruct;
    {
        _mongoc_array_init(&arr_mystruct, sizeof(struct mystruct));

        struct mystruct s;
        s.x = 1;
        s.y = 2;
        _mongoc_array_append_val(&arr_mystruct, s);

        s.x = 3;
        s.y = 4;
        _mongoc_array_append_val(&arr_mystruct, s);

        // Assert values are in array.
        struct mystruct got;
        got = _mongoc_array_index(&arr_mystruct, struct mystruct, 0);
        assert(got.x == 1);
        assert(got.y == 2);
        got = _mongoc_array_index(&arr_mystruct, struct mystruct, 1);
        assert(got.x == 3);
        assert(got.y == 4);
    }

    // Test with an array of typedef'ed int32.
    mongoc_array_t arr_i32alias;
    {
        _mongoc_array_init(&arr_i32alias, sizeof(i32alias));

        // Append the values [1,2].
        i32alias i32;
        i32 = 1;
        _mongoc_array_append_val(&arr_i32alias, i32);
        i32 = 2;
        _mongoc_array_append_val(&arr_i32alias, i32);

        // Assert values are in array.
        i32alias got;
        got = _mongoc_array_index(&arr_i32alias, i32alias, 0);
        assert(got == 1);
        got = _mongoc_array_index(&arr_i32alias, i32alias, 1);
        assert(got == 2);
    }

    // Test with an array of pointers.
    mongoc_array_t arr_i32ptr;
    {
        _mongoc_array_init(&arr_i32ptr, sizeof(int32_t*));

        // Append the values [1,2].
        int32_t i32_1, i32_2;
        i32_1 = 1;
        i32_2 = 2;
        int32_t *p1 = &i32_1;
        int32_t *p2 = &i32_2;
        _mongoc_array_append_val(&arr_i32ptr, p1);
        _mongoc_array_append_val(&arr_i32ptr, p2);

        // Assert values are in array.
        int32_t* got;
        got = _mongoc_array_index(&arr_i32ptr, int32_t*, 0);
        assert(*got == 1);
        got = _mongoc_array_index(&arr_i32ptr, int32_t*, 1);
        assert(*got == 2);
    }

    return 0;
}