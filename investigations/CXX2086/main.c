#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For usleep
#include <stdatomic.h>

#define MONGO_URI "mongodb://localhost:27017"
#define DATABASE "testdb"
#define COLLECTION "testcollection"
#define NUM_THREADS 4

// Atomic counters for operations
atomic_int insert_count = 0;
atomic_int find_count = 0;
atomic_int update_count = 0;
atomic_int delete_count = 0;

// Function declarations
void* insertOne(void* pool_void);
void* findOne(void* pool_void);
void* updateOne(void* pool_void);
void* deleteOne(void* pool_void);
void* reportThroughput(void* arg);

int main() {
    mongoc_init();

    printf ("mongoc_get_version()=%s\n", mongoc_get_version());

    mongoc_uri_t* uri = mongoc_uri_new_with_error(MONGO_URI, NULL);
    mongoc_client_pool_t* pool = mongoc_client_pool_new(uri);
    pthread_t threads[NUM_THREADS];
    pthread_t report_thread;

    pthread_create(&threads[0], NULL, insertOne, pool);
    pthread_create(&threads[1], NULL, findOne, pool);
    pthread_create(&threads[2], NULL, updateOne, pool);
    pthread_create(&threads[3], NULL, deleteOne, pool);
    pthread_create(&report_thread, NULL, reportThroughput, NULL);

    // Join threads (for this example, they should run indefinitely)
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(report_thread, NULL);

    mongoc_client_pool_destroy(pool);
    mongoc_uri_destroy(uri);
    mongoc_cleanup();

    return 0;
}

void* insertOne(void* pool_void) {
    mongoc_client_pool_t* pool = (mongoc_client_pool_t*)pool_void;
    mongoc_client_t* client;
    mongoc_collection_t* collection;
    bson_t* document;
    bson_error_t error;

    while (1) {
        client = mongoc_client_pool_pop(pool);
        collection = mongoc_client_get_collection(client, DATABASE, COLLECTION);

        document = BCON_NEW("name", BCON_UTF8("Alice"));
        if (mongoc_collection_insert_one(collection, document, NULL, NULL, &error)) {
            atomic_fetch_add(&insert_count, 1);
        } else {
            fprintf(stderr, "Insert failed: %s\n", error.message);
        }

        bson_destroy(document);
        mongoc_collection_destroy(collection);
        mongoc_client_pool_push(pool, client);
    }

    return NULL;
}

void* findOne(void* pool_void) {
    mongoc_client_pool_t* pool = (mongoc_client_pool_t*)pool_void;
    mongoc_client_t* client;
    mongoc_collection_t* collection;
    bson_t* query;
    const bson_t* doc;
    mongoc_cursor_t* cursor;

    while (1) {
        client = mongoc_client_pool_pop(pool);
        collection = mongoc_client_get_collection(client, DATABASE, COLLECTION);

        query = bson_new();
        cursor = mongoc_collection_find_with_opts(collection, query, NULL, NULL);

        if (mongoc_cursor_next(cursor, &doc)) {
            atomic_fetch_add(&find_count, 1);
        }

        bson_destroy(query);
        mongoc_cursor_destroy(cursor);
        mongoc_collection_destroy(collection);
        mongoc_client_pool_push(pool, client);
    }

    return NULL;
}

void* updateOne(void* pool_void) {
    mongoc_client_pool_t* pool = (mongoc_client_pool_t*)pool_void;
    mongoc_client_t* client;
    mongoc_collection_t* collection;
    bson_t* query;
    bson_t* update;
    bson_error_t error;

    while (1) {
        client = mongoc_client_pool_pop(pool);
        collection = mongoc_client_get_collection(client, DATABASE, COLLECTION);

        query = BCON_NEW("name", BCON_UTF8("Alice"));
        update = BCON_NEW("$set", "{", "name", BCON_UTF8("Bob"), "}");

        if (mongoc_collection_update_one(collection, query, update, NULL, NULL, &error)) {
            atomic_fetch_add(&update_count, 1);
        }

        bson_destroy(query);
        bson_destroy(update);
        mongoc_collection_destroy(collection);
        mongoc_client_pool_push(pool, client);
    }

    return NULL;
}

void* deleteOne(void* pool_void) {
    mongoc_client_pool_t* pool = (mongoc_client_pool_t*)pool_void;
    mongoc_client_t* client;
    mongoc_collection_t* collection;
    bson_t* query;
    bson_error_t error;

    while (1) {
        client = mongoc_client_pool_pop(pool);
        collection = mongoc_client_get_collection(client, DATABASE, COLLECTION);

        query = BCON_NEW("name", BCON_UTF8("Bob"));

        if (mongoc_collection_delete_one(collection, query, NULL, NULL, &error)) {
            atomic_fetch_add(&delete_count, 1);
        }

        bson_destroy(query);
        mongoc_collection_destroy(collection);
        mongoc_client_pool_push(pool, client);
    }

    return NULL;
}

void* reportThroughput(void* arg) {
    while (1) {
        sleep(1);
        int insert_ops = atomic_exchange(&insert_count, 0);
        int find_ops = atomic_exchange(&find_count, 0);
        int update_ops = atomic_exchange(&update_count, 0);
        int delete_ops = atomic_exchange(&delete_count, 0);

        printf("Throughput (ops/sec): Insert: %d, Find: %d, Update: %d, Delete: %d\n", 
               insert_ops, find_ops, update_ops, delete_ops);
    }

    return NULL;
}
