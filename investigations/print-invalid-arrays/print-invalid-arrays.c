#include <mongoc/mongoc.h>

#define FAILF(fmt, ...)                                                        \
  if (1) {                                                                     \
    fprintf(stderr, "%s:%d FAIL: " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); \
    exit(1);                                                                   \
  } else                                                                       \
    ((void)0)

#define FAIL(msg) FAILF("%s", msg);

typedef struct {
  uint32_t depth;
  char *path;
  bool is_array;
} visitor_state_t;

static const char *type_to_string(bson_type_t bson_type) {
  switch (bson_type) {
  case BSON_TYPE_EOD:
    return "eod";
  case BSON_TYPE_DOUBLE:
    return "double";
  case BSON_TYPE_UTF8:
    return "utf8";
  case BSON_TYPE_DOCUMENT:
    return "document";
  case BSON_TYPE_ARRAY:
    return "array";
  case BSON_TYPE_BINARY:
    return "binary";
  case BSON_TYPE_UNDEFINED:
    return "undefined";
  case BSON_TYPE_OID:
    return "oid";
  case BSON_TYPE_BOOL:
    return "bool";
  case BSON_TYPE_DATE_TIME:
    return "date_time";
  case BSON_TYPE_NULL:
    return "null";
  case BSON_TYPE_REGEX:
    return "regex";
  case BSON_TYPE_DBPOINTER:
    return "dbpointer";
  case BSON_TYPE_CODE:
    return "code";
  case BSON_TYPE_SYMBOL:
    return "symbol";
  case BSON_TYPE_CODEWSCOPE:
    return "codewscope";
  case BSON_TYPE_INT32:
    return "int32";
  case BSON_TYPE_TIMESTAMP:
    return "timestamp";
  case BSON_TYPE_INT64:
    return "int64";
  case BSON_TYPE_DECIMAL128:
    return "decimal128";
  case BSON_TYPE_MAXKEY:
    return "maxkey";
  case BSON_TYPE_MINKEY:
    return "minkey";
  }
}

void visit(visitor_state_t *state, bson_iter_t iter) {
  state->depth++;

  bool is_array = state->is_array;
  bool has_invalid_keys = false;
  bson_iter_t start = iter;

  size_t index = 0;
  while (bson_iter_next(&iter)) {
    const char *key = bson_iter_key(&iter);
    const bson_type_t bson_type = bson_iter_type(&iter);

    if (is_array) {
      // Check for sequential numeric keys.
      char *expect = bson_strdup_printf("%zu", index);
      if (0 != strcmp(key, expect)) {
        has_invalid_keys = true;
      }
    }
    index++;

    if (state->depth == 1) {
      printf("'%s': [%s]\n", key, type_to_string(bson_type));
    } else {
      printf("'%s.%s': [%s]\n", state->path, key, type_to_string(bson_type));
    }

    if (BSON_ITER_HOLDS_DOCUMENT(&iter) || BSON_ITER_HOLDS_ARRAY(&iter)) {
      // Recurse.

      bson_iter_t child;
      bson_iter_recurse(&iter, &child);

      visitor_state_t old_state = *state; // Copy old state.
      if (state->depth == 1) {
        state->path = bson_strdup_printf("%s", key);
      } else {
        state->path = bson_strdup_printf("%s.%s", state->path, key);
      }
      state->depth++;
      state->is_array = BSON_ITER_HOLDS_ARRAY(&iter);

      visit(state, child);
      bson_free(state->path);
      *state = old_state; // Restore old state.
    }
  }

  if (has_invalid_keys) {
    printf("'%s' has invalid array keys: ", state->path);
    bool first = false;
    iter = start;
    while (bson_iter_next(&iter)) {
      if (first) {
        printf(", ");
      } else {
        first = true;
      }
      printf("%s", bson_iter_key(&iter));
    }
    printf("\n");
  }
}

int main(int argc, char *argv[]) {
  mongoc_init();

  if (argc != 2) {
    FAILF("Usage: %s <file>.bson", argv[0]);
  }

  const char *const path = argv[1];

  bson_error_t error;
  bson_reader_t *reader = bson_reader_new_from_file(path, &error);
  if (!reader) {
    FAILF("Failed to open file '%s': %s", path, error.message);
  }

  bool reached_eof = false;
  while (true) {
    const bson_t *bson = bson_reader_read(reader, &reached_eof);
    if (!bson) {
      if (reached_eof) {
        break;
      } else {
        FAILF("Failed to read document from file '%s'", path);
      }
    }

    bson_iter_t iter;
    BSON_ASSERT(bson_iter_init(&iter, bson));
    visitor_state_t state = {.depth = 0, .path = "", .is_array = false};
    visit(&state, iter);
  }

  bson_reader_destroy(reader);

  mongoc_cleanup();
}
