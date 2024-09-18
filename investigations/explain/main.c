#include <mongoc/mongoc.h>

int main()
{
    mongoc_init();

    mongoc_client_t *client = mongoc_client_new("mongodb://localhost:27017");

    bson_t *cmd = BCON_NEW(
        "aggregate", "coll",
        "explain", BCON_BOOL(true),
        "pipeline",
        "[",
        "{", "$match", "{", "cuisine", BCON_UTF8("Bakery"), "}", "}",
        "{", "$group", "{",
        "_id", BCON_UTF8("$borough"), "count", "{", "$sum", BCON_INT32(1), "}", "}",
        "}",
        "]");

    bson_t reply;
    bson_error_t error;
    if (!mongoc_client_command_simple(client, "db", cmd, NULL, &reply, &error))
    {
        printf("error running aggregate with explain: %s\n", error.message);
    }
    else
    {
        char *str = bson_as_relaxed_extended_json(&reply, NULL);
        printf("got reply: %s\n", str);
        bson_free(str);
    }

    bson_destroy(&reply);
    bson_destroy(cmd);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}
