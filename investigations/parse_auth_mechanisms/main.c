/*
To build on macOS:
brew install mongo-c-driver
clang -o parse_auth_mechanisms main.c $(pkg-config --libs --cflags libmongoc-static-1.0)

To run:
$ MONGODB_URI="mongodb://localhost:27017/?authMechanismProperties=a:b,b:1,c:{}" ./parse_auth_mechanisms
mongoc_get_version=1.25.0
parsed authMechanismProperties as: { "a" : "b", "b" : "1", "c" : "{}" }
*/

#include <mongoc/mongoc.h>

int main()
{
    char *uristr;

    uristr = getenv("MONGODB_URI");
    if (!uristr)
    {
        uristr = "mongodb://localhost:27017";
    }
    mongoc_init();

    printf("mongoc_get_version=%s\n", mongoc_get_version());
    bson_error_t error;
    mongoc_uri_t *uri = mongoc_uri_new_with_error(uristr, &error);
    if (!uri)
    {
        fprintf(stderr, "failed to parse URI '%s': %s\n", uristr, error.message);
        return 1;
    }

    bson_t mechanism_properties = BSON_INITIALIZER;
    mongoc_uri_get_mechanism_properties(uri, &mechanism_properties);
    char *mechanism_properties_str = bson_as_json(&mechanism_properties, NULL);

    printf("parsed authMechanismProperties as: %s\n", mechanism_properties_str);
    bson_free(mechanism_properties_str);
    bson_destroy(&mechanism_properties);
    mongoc_cleanup();
}
