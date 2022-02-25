#include <mongoc/mongoc.h>

int main () {
    char *uristr;

    uristr = getenv ("MONGODB_URI");
    if (!uristr) {
        uristr = "mongodb://localhost:27017";
    }
    mongoc_init ();

    mongoc_cleanup ();
}
