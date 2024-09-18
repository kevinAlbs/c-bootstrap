#include <mongoc/mongoc.h>

int main()
{

    mongoc_init();

    // Build query vector array.
    bson_error_t error;
    bson_t query_vector;
    {
        const char *as_json = BSON_STR([
            1.0, 2.0, 3.0 /* and more */
        ]);
        if (!bson_init_from_json(&query_vector, as_json, -1, &error))
        {
            printf("failed to build query vector: %s\n", error.message);
            return 1;
        }
    }

    // clang-format off
    bson_t *pipeline = BCON_NEW(
        "pipeline",
        "[",
            "{",
                "$vectorSearch", "{", 
                    "index", BCON_UTF8("vector_index"), 
                    "path", BCON_UTF8("plot_embedding"), 
                    "query_vector", BCON_ARRAY (&query_vector),
                    "numCandidates", BCON_INT32(150), 
                    "limiti", BCON_INT32(10), 
                "}", 
            "}",
            "{",
                "$project", "{", 
                    "title", BCON_BOOL(true),
                    "plot", BCON_BOOL(true),
                    "_id", BCON_BOOL(false),
                "}",
            "}",
        "]"
    );
    // clang-format on
    char *str = bson_as_relaxed_extended_json(pipeline, NULL);
    printf("%s\n", str);
    bson_free(str);

    bson_destroy(&query_vector);
    bson_destroy(pipeline);
    mongoc_cleanup();
}
