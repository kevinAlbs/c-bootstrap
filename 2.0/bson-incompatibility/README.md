# `bson_t` alignment incompatibility

`libdep.so` is built with `bson1.h` (128-byte aligned).

`libdep.so` defines a struct:

```c
typedef struct
{
   int placeholder;
   bson_t bson; // Offset of `bson` depends on the alignment of `bson_t`.
} dep_t;
```

`app.out` is built with `bson2.h` (default aligned).

`app.out` depends on `libdep.so`.

`app.out` has a different data layout for `dep_t` than `libdep.so`.

To observe, run:

```bash
docker build . -t bson-incompatibility
docker run --rm -it bson-incompatibility
```

Produces output like:
```
app has dep_t::bson at offset: 4
dep has dep_t::bson at offset: 128
Error: expected 123, but got: -842047528
```
