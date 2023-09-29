#include <mongoc/mongoc.h>

#include "../../util/monitoring.h"

// Set `VERBOSE` to 1 to print all command monitoring events.
#define VERBOSE 0

#define ASSERT BSON_ASSERT
#define ASSERTF(stmt, ...)                                                           \
    if (!(stmt))                                                                     \
    {                                                                                \
        fprintf(stderr, "error at statement \"%s\" on line: %d\n", #stmt, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                                                \
        fprintf(stderr, "\n");                                                       \
        abort();                                                                     \
    }                                                                                \
    else                                                                             \
        (void)0

int main()
{
    mongoc_client_t *client;
    char *uristr;
    bson_error_t error;

    uristr = getenv("MONGODB_URI");
    if (!uristr)
    {
        uristr = "mongodb://localhost:27017";
    }
    mongoc_init();

    MONGOC_DEBUG("mongoc_get_version=%s", mongoc_get_version());
    client = mongoc_client_new(uristr);

    if (VERBOSE)
    {
        // Print command monitoring events.
        mongoc_apm_callbacks_t *cbs = mongoc_apm_callbacks_new();
        set_command_monitors(cbs, false /* started only */);
        ASSERT(mongoc_client_set_apm_callbacks(client, cbs, NULL /* context */));
    }

    mongoc_collection_t *coll = mongoc_client_get_collection(client, "db", "coll");
    // Use majority write concern. Change streams read with majority read concern.
    {
        mongoc_write_concern_t *wc = mongoc_write_concern_new();
        mongoc_write_concern_set_w(wc, MONGOC_WRITE_CONCERN_W_MAJORITY);
        mongoc_collection_set_write_concern(coll, wc);
        mongoc_write_concern_destroy(wc);
    }

    printf("Create a change stream and invalidate ... begin\n");
    {
        bson_t *pipeline = bson_new();

        // Insert a document into the collection to ensure the collection is created.
        {
            bson_t *doc = BCON_NEW("foo", "bar");
            ASSERTF(mongoc_collection_insert_one(coll, doc, NULL /* opts */, NULL /* reply */, &error), "error in insert_one: %s", error.message);
            bson_destroy(doc);
        }

        mongoc_change_stream_t *cs = mongoc_collection_watch(coll, pipeline, NULL /* opts */);

        ASSERTF(mongoc_collection_drop(coll, &error), "error in drop: %s", error.message);

        // Iterate until the next event. Expect "drop" event.
        {
            const bson_t *event;
            bool found_event = false;
            while (!found_event)
            {
                found_event = mongoc_change_stream_next(cs, &event);
                if (!found_event)
                {
                    ASSERTF(!mongoc_change_stream_error_document(cs, &error, NULL /* document */), "error: %s", error.message);
                }
            }
            bson_iter_t iter;
            ASSERT(bson_iter_init_find(&iter, event, "operationType"));
            const char *operationType = bson_iter_utf8(&iter, NULL);
            ASSERTF(0 == strcmp(operationType, "drop"), "expected operationType='drop', got '%s'", operationType);
        }

        // Iterate until the next event. Expect "invalidate" event.
        {
            const bson_t *event;
            bool found_event = false;
            while (!found_event)
            {
                found_event = mongoc_change_stream_next(cs, &event);
                if (!found_event)
                {
                    ASSERTF(!mongoc_change_stream_error_document(cs, &error, NULL /* document */), "error: %s", error.message);
                }
            }
            bson_iter_t iter;
            ASSERT(bson_iter_init_find(&iter, event, "operationType"));
            const char *operationType = bson_iter_utf8(&iter, NULL);
            ASSERTF(0 == strcmp(operationType, "invalidate"), "expected operationType='invalidate', got '%s'", operationType);
        }

        // Iterate until the next error.
        {
            const bson_t *event;
            bool found_error = false;
            while (!found_error)
            {
                ASSERTF(!mongoc_change_stream_next(cs, &event), "expected no event, got %s", bson_as_json(event, NULL));
                found_error = mongoc_change_stream_error_document(cs, &error, NULL /* document */);
            }
            ASSERTF(strstr(error.message, "Attempting to resume"), "Expected error to contain 'Attempting to resume' but got: %s", error.message);
        }

        mongoc_change_stream_destroy(cs);
        bson_destroy(pipeline);
    }
    printf("Create a change stream and invalidate ... end\n");

    mongoc_collection_destroy(coll);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}
