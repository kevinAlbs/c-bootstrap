#include <mongoc/mongoc.h>

int main()
{

    mongoc_init();

    // Build query vector array.
    bson_t query_vector;
    {
        bson_array_builder_t *builder = bson_array_builder_new();
        bson_array_builder_append_double(builder, 1.0);
        bson_array_builder_append_double(builder, 2.0);
        // And more...

        bson_array_builder_build(builder, &query_vector);
        bson_array_builder_destroy(builder);
    }

    // clang-format off
    bson_t *pipeline = BCON_NEW(
        "pipeline",
        "[",
            "{",
                "$vectorSearch", "{", 
                    "index", BCON_UTF8("vector_index"), 
                    "path", BCON_UTF8("plot_embedding"), 
                    "query_vector", BCON_ARRAY(&query_vector),
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
