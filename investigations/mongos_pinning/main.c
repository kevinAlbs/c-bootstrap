#include <mongoc/mongoc.h>

int main () {
    char* uristr;
    mongoc_uri_t *uri;
    mongoc_client_pool_t *pool;
    mongoc_client_t *client;
    mongoc_client_session_t *session;
    bson_t *opts;
    mongoc_collection_t *coll;
    bson_t *doc_to_insert;
    bson_error_t error;
    uint32_t server_id;

    mongoc_init ();

    uristr = getenv ("MONGODB_URI");
    if (!uristr) {
        MONGOC_ERROR ("Set MONGODB_URI environment variable");
        return EXIT_FAILURE;
    }

    uri = mongoc_uri_new (uristr);
    pool = mongoc_client_pool_new (uri);
    client = mongoc_client_pool_pop (pool);
    session = mongoc_client_start_session (client, NULL /* opts */, &error);
    if (!session) {
        MONGOC_ERROR ("mongoc_client_start_session error: %s", error.message);
        return EXIT_FAILURE;
    }

    if (!mongoc_client_session_start_transaction (session, NULL /* opts */, &error)) {
        MONGOC_ERROR ("mongoc_client_session_start_transaction error: %s", error.message);
        return EXIT_FAILURE;
    }

    opts = bson_new ();
    coll = mongoc_client_get_collection (client, "foo", "bar");
    server_id = mongoc_client_session_get_server_id (session);
    MONGOC_DEBUG ("before insert, server_id = %" PRIu32, server_id);
    doc_to_insert = BCON_NEW ("x", BCON_INT32(1));
    if (!mongoc_collection_insert_one (coll, doc_to_insert, opts, NULL /* reply */, &error)) {
        MONGOC_ERROR ("mongoc_collection_insert_one error: %s", error.message);
        return EXIT_FAILURE;
    }
    MONGOC_DEBUG ("after insert, server_id = %" PRIu32, server_id);

    if (!mongoc_client_session_commit_transaction (session, NULL /* reply */, &error)) {
        MONGOC_ERROR ("mongoc_client_session_commit_transaction error: %s", error.message);
        return EXIT_FAILURE;
    }
    
    bson_destroy (doc_to_insert);
    mongoc_collection_destroy (coll);
    bson_destroy (opts);
    mongoc_client_pool_push (pool, client);
    mongoc_client_pool_destroy (pool);
    mongoc_uri_destroy (uri);
    mongoc_cleanup ();
}
