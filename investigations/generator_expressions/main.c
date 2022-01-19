#include <mongoc/mongoc.h>

int main () {
    mongoc_init ();

    mongoc_cleanup ();
}
