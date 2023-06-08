#include <mongoc/mongoc.h>
// Q: Does replace operation apply a pipeline as an update?
// A: Yes.

#define FAIL_NOW(...)                                     \
    if (1)                                                \
    {                                                     \
        fprintf(stderr, "error on line: %d\n", __LINE__); \
        fprintf(stderr, __VA_ARGS__);                     \
        fprintf(stderr, "\n");                            \
        abort();                                          \
    }                                                     \
    else                                                  \
        (void)0

#define ASSERT_BSON_EQUAL(expect, got)                                                \
    if (1)                                                                            \
    {                                                                                 \
        if (!bson_equal(expect, got))                                                 \
        {                                                                             \
            char *got_str = bson_as_canonical_extended_json(got, NULL);               \
            char *expect_str = bson_as_canonical_extended_json(expect, NULL);         \
            FAIL_NOW("expected document: %s\ngot document: %s", expect_str, got_str); \
            bson_free(expect_str);                                                    \
            bson_free(got_str);                                                       \
        }                                                                             \
    }                                                                                 \
    else                                                                              \
        (void)0

static bson_t *
find_one_by_id(mongoc_collection_t *coll, int32_t id)
{
    bson_error_t error;
    bson_t *filter = BCON_NEW("_id", BCON_INT32(id));
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(coll, filter, NULL /* opts */, NULL /* read_prefs */);
    const bson_t *got;
    if (!mongoc_cursor_next(cursor, &got))
    {
        if (mongoc_cursor_error(cursor, &error))
        {
            FAIL_NOW("expected to find document, but got error: %s", error.message);
        }
        FAIL_NOW("expected to find document, but found none");
    }
    bson_t *ret = bson_copy(got);
    mongoc_cursor_destroy(cursor);
    return ret;
}

int main()
{
    bson_error_t error;
    mongoc_client_t *client;
    char *uristr;

    uristr = getenv("MONGODB_URI");
    if (!uristr)
    {
        uristr = "mongodb://localhost:27017";
    }
    mongoc_init();

    printf("mongoc_get_version=%s\n", mongoc_get_version());
    client = mongoc_client_new(uristr);

    // Insert document { "_id": 1 } into `coll`.
    mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");
    {
        // Drop collection to clean up prior test data.
        mongoc_collection_drop(coll, NULL);
        bson_t *doc = BCON_NEW("_id", BCON_INT32(1));
        if (!mongoc_collection_insert_one(coll, doc, NULL /* opts */, NULL /* reply */, &error))
        {
            FAIL_NOW("failed to insert: %s", error.message);
        }
        bson_destroy(doc);
    }

    // Construct a `bson_t` that could be used as an update pipeline.
    bson_t *pipeline;
    pipeline = BCON_NEW("0", "{", "$addFields", "{", "x", BCON_INT32(1), "}", "}");

    // Try to replace the document with pipeline.
    {
        bson_t *selector = BCON_NEW("_id", BCON_INT32(1));
        if (!mongoc_collection_replace_one(coll, selector, pipeline, NULL /* opts */, NULL /* reply */, &error))
        {
            FAIL_NOW("failed to replace: %s", error.message);
        }

        // Expect resulting document to be: { "0": { "$addFields": { "x": 1 } } }?
        bson_t *expect = BCON_NEW("0", "{", "$addFields", "{", "x", BCON_INT32(1), "}", "}");
        bson_t *got = find_one_by_id(coll, 1);
        ASSERT_BSON_EQUAL(expect, got);
        bson_destroy(got);
        bson_destroy(expect);
        bson_destroy(selector);
    }

    mongoc_collection_destroy(coll);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}

/*
Sample output:
mongoc_get_version=1.23.2
error on line: 95
expected document: { "0" : { "$addFields" : { "x" : { "$numberInt" : "1" } } } }
got document: { "_id" : { "$numberInt" : "1" }, "x" : { "$numberInt" : "1" } }
./run.sh: line 1: 61284 Abort trap: 6           ./cmake-build/main.out
*/
