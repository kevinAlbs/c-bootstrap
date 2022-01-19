#include <mongoc/mongoc.h>

int main () {
    mongoc_init ();

    MONGOC_DEBUG ("rand=%d", rand());
    MONGOC_DEBUG ("rand=%d", rand());
    MONGOC_DEBUG ("rand=%d", rand());

    mongoc_cleanup ();
}
