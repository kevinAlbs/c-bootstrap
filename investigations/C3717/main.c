#include <mongoc.h>

int main () {
    mongoc_init ();

    MONGOC_DEBUG ("mongoc_get_version=%s", mongoc_get_version ());

    mongoc_cleanup ();
}
