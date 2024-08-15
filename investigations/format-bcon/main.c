#include <mongoc/mongoc.h>

#define BCON_SUBDOC(...) "{", __VA_ARGS__, "}"
#define BCON_SUBARR(...) "[", __VA_ARGS__, "]"

int main()
{
    mongoc_client_t *client;
    char *uristr;

    bson_t *doc;
    {
        doc = BCON_NEW("foo", "{", "int", BCON_INT32(1), "array", "[",
                       BCON_INT32(100), "{", "sub", BCON_UTF8("value"), "}", "]",
                       "}");
    }

    {
        doc = BCON_NEW(
            "foo",
            BCON_SUBDOC("int", BCON_INT32(1), "array",
                        BCON_SUBARR(BCON_INT32(100),
                                    BCON_SUBDOC("sub", BCON_UTF8("value")))));
    }
    bson_t *pipeline;
    pipeline =
        BCON_NEW("pipeline",
                 BCON_SUBARR(BCON_SUBDOC("$match", BCON_SUBDOC("adoptable",
                                                               BCON_BOOL(true))),
                             BCON_SUBDOC("$count", BCON_UTF8("adoptableCount"))));

    mongoc_client_destroy(client);
    mongoc_cleanup();
}
