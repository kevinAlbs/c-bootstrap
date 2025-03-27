#include <mongoc/mongoc.h>

int main () {
    mongoc_init ();
    printf ("Using libmongoc: %s\n", mongoc_get_version ());
    mongoc_cleanup ();
}
