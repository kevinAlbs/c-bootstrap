#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include <mongoc.h>

#define DBNAME "longdatabasename78921234567893123456789412345678951234567896123"
#define COLNAME "batchtest"
#define NAMESPACE DBNAME "." COLNAME
enum optypes{insert_op, delete_op, update_op};

void printConstants() {
    printf("\n**********\n");
    printf("Testing with: \ndatabase name: %s,(%lu)\n", DBNAME, strlen(DBNAME));
    printf("collection name: %s(%lu)\n", COLNAME, strlen(COLNAME));
    printf("namespace: %s(%lu)\n", NAMESPACE, strlen(NAMESPACE));
    printf("**********\n\n");
}

bool runCommand(mongoc_client_t *client, const char *db_name, const char *json, char **retstring, bson_t *retval)
{
  bson_error_t error;
  bson_t doc;
  bson_t *reply;
  char *s;
  char **str;

  if (retstring == NULL) {
      str = &s;
  } else {
      str = retstring;
  }

  if (retval == NULL) {
      reply = &doc;
  } else {
      reply = retval;
  }

  if (!client) {
      fprintf(stderr, "[ERROR] no client to run %s\n", json);
      return false;
  }

  bson_t *command = bson_new_from_json((const uint8_t *)json, -1, &error);
  if (!command) {
      fprintf(stderr, "[ERROR] \%s\n", error.message);
      fprintf(stderr, "%s\n", json);
      return false;
  }

  s = bson_as_json(command, NULL);
  printf("[COMMAND] \%s\n", s);
  bson_free(s);

  if (!mongoc_client_command_simple(client, db_name, command, NULL, reply, &error)) {
      fprintf(stderr, "[ERROR] \%s\n", error.message);
      return false;
  }

  *str = bson_as_json(reply, NULL);
  printf("[REPLY] \%s\n", *str);

  if (retstring == NULL) {
      bson_free(*str);
  }
  
  return true;
}

bool runDataOp(enum optypes optype, mongoc_client_t* client, const char *db_name, const char *collection_name, const char *json)
{
	bson_error_t error;
	char *str;
	bool retval;

	if (!client) {
        fprintf(stderr, "[ERROR] no client to run %s\n", json);
        return false;
	}

	mongoc_collection_t *collection = mongoc_client_get_collection(client, db_name, collection_name);
	if (!collection) {
        fprintf(stderr, "[ERROR] unable to get collection %s\n", collection_name);
        return false;
	}

	bson_t *command = bson_new_from_json((const uint8_t *)json, -1, &error);
	if (!command) {
		fprintf(stderr, "[ERROR] \%s\n", error.message);
		return false;
	}

	str = bson_as_json(command, NULL);

    switch (optype) {
        case insert_op:
            printf("[DATA] insert %s into %s.%s\n", str, db_name, collection_name);
            retval = mongoc_collection_insert(collection, MONGOC_INSERT_NONE, command, NULL, &error);
            break;
        case delete_op:
            printf("[DATA] delete %s from %s.%s\n", str, db_name, collection_name);
            retval = mongoc_collection_remove(collection, MONGOC_REMOVE_NONE, command, NULL, &error);
            break;
        case update_op:
            //TODO
            break;
    }

	if (!retval) {
		fprintf(stderr, "[ERROR] \%s\n", error.message);
		return false;
	}

	mongoc_collection_destroy(collection);
	bson_free(str);
	return true;
}

bool waitForMaster(mongoc_client_t* client) {
    char *str;

	while (true) {
		const char *isMaster = "{ \"isMaster\": 1 }";
		if (!runCommand(client, "admin", isMaster, &str, NULL)) {
			return false;
		}

		if (strstr(str, "\"ismaster\" : true") != NULL) {
			break;
		}
		sleep(1);
	}

    bson_free(str);
    return true;
}



bool initReplicaSet(const char *uri) 
{
    bool retval;
    mongoc_client_t *client = mongoc_client_new(uri);

	const char *replInit = "{ \"replSetInitiate\": { \"_id\": \"5E79011A-7B0C-487B-918E-F3CC147383CD\","
                                                    "\"members\": ["
                                                        "{ \"_id\": 0,"
                                                          "\"host\": \"127.0.0.1:27017\","
                                                          "\"votes\": 1,"
                                                          "\"priority\": 1}"
                                                    "]}}";
	if (!runCommand(client, "admin", replInit, NULL, NULL)) {
		return false;
    }
    retval = waitForMaster(client);
    
	mongoc_client_destroy(client);
    return retval;
}

void exhaust_cursor(mongoc_cursor_t *cursor) 
{
    const bson_t *result;
    int rescount=0;

    while (mongoc_cursor_next(cursor, &result)) {
        rescount++;
        printf("\rfound: %i, more: %s",rescount, mongoc_cursor_more(cursor)?"True":"False");
    }
    printf("\n");

    printf("Found: %d documents\n", rescount);
}

static int numGetMoreSent = 0;

static void command_started (mongoc_apm_command_started_t* event) {
    if (0 == strcmp ("getMore", mongoc_apm_command_started_get_command_name (event))) {
        numGetMoreSent++;
    }
}

int main(int argv, char **argc)
{
    mongoc_uri_t *uri;
    mongoc_client_pool_t *pool;
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    mongoc_bulk_operation_t *bulk;
    mongoc_cursor_t *cursor, *cursor2;
    bson_t *filter, *opts, *doc, *command;
    bson_error_t error;
    char *strbuff;
    int buffsize = 16 * 1024;
    int i;

	const char *standaloneUri = "mongodb://localhost:27017";

	mongoc_init();

    /*
    if (!initReplicaSet(standaloneUri)) {
        return EXIT_FAILURE;
    }
    */

	uri = mongoc_uri_new(standaloneUri);
	pool = mongoc_client_pool_new(uri);
	mongoc_uri_destroy(uri);

    // replica set is up and ready, start real test

	client = mongoc_client_pool_pop(pool);
    
    printConstants();
   
    collection = mongoc_client_get_collection(client, DBNAME, COLNAME);

    // Generate documents large enought to require 3 batches in the results
    strbuff = malloc(buffsize);
    assert(strbuff);

    memset(strbuff, 'A', buffsize);
    strbuff[buffsize - 1]=0;

    doc = bson_new();
    BSON_APPEND_UTF8(doc, "key", strbuff);
    bulk = mongoc_collection_create_bulk_operation_with_opts (collection, NULL);

    printf("Insert 2000 documents with %d-btye string\n",buffsize);
    for (i = 0; i < 2000; i++) {
        mongoc_bulk_operation_insert (bulk, doc);
    }

    assert(mongoc_bulk_operation_execute (bulk, NULL, &error));
    
    bson_destroy(doc);
    free(strbuff);

    filter = bson_new();
    opts = bson_new();
    BSON_APPEND_BOOL(opts,"exhaust",true);
    doc = bson_new();
    //BSON_APPEND_INT32(opts,"limit",0);

    //test with manual find command
    printf("--------- find command -----------\n");
    command = bson_new_from_json((const unsigned char *)"{\"find\": \"" COLNAME "\", \"filter\":{}, \"batchSize\":1}",-1, NULL);
    mongoc_client_command_simple(client, DBNAME, command, NULL, doc, &error);
    cursor = mongoc_cursor_new_from_command_reply_with_opts(client, doc, opts);
    exhaust_cursor(cursor);
    mongoc_cursor_destroy(cursor);

    //test with find_with_opts
    printf("--------- mongoc_collection_find_with_opts -----------\n");
    cursor2 = mongoc_collection_find_with_opts(collection, filter, opts, NULL);
    exhaust_cursor(cursor2);
    mongoc_cursor_destroy(cursor2);


    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    return EXIT_SUCCESS;
}