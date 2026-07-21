// hello-amongoc.c - Minimal "hello world" for the async C driver (mongoac).

#include <bson/bson.h>

#include <mongoac/client.h>
#include <mongoac/error.h>
#include <mongoac/future.h>
#include <mongoac/runtime.h>

#include <stdio.h>
#include <stdlib.h>

#define CHECK_OK(msg)                                                          \
  if (mongoac_error_code(error) != MONGOAC_ERROR_CODE_OK) {                    \
    fprintf(stderr, "%s: %s\n", (msg), mongoac_error_message(error));          \
    goto end;                                                                  \
  }

int main(void) {
  int rc = EXIT_FAILURE;
  mongoac_client_t *client = NULL;
  mongoac_future_t *future = NULL;
  bson_t *result = NULL;
  char *json = NULL;

  mongoac_error_t *error = mongoac_error_new();

  client = mongoac_client_new("mongodb://localhost:27017", error);
  CHECK_OK("client creation failed");
  // client owns an async runtime:
  mongoac_runtime_t *runtime = mongoac_client_get_runtime(client);

  // Operations return futures:
  future = mongoac_client_list_database_names_async(client, NULL, NULL, error);
  CHECK_OK("failed to start listDatabases");

  // poll() is a non-blocking check:
  while (!mongoac_future_poll(future)) {
    // make_progress() blocks to advance runtime:
    mongoac_runtime_make_progress(runtime);
    // make_progress() can run on any thread.
  }

  result = mongoac_future_get_bson(future, error);
  CHECK_OK("listDatabases failed");

  json = bson_as_relaxed_extended_json(result, NULL);
  printf("Database names:\n%s\n", json);
  rc = EXIT_SUCCESS;

end:
  bson_free(json);
  bson_destroy(result);
  mongoac_future_destroy(future);
  mongoac_client_destroy(client);
  mongoac_error_destroy(error);
  return rc;
}
