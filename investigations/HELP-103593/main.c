#include <inttypes.h>
#include <mongoc/mongoc.h>
#include <stdio.h>
#include <time.h>

// HELP-103593: does server selection block on a down secondary's hello
// even when the primary is known and the op targets the primary?
// Run against a 3-node RS; SIGSTOP a secondary mid-run (see run_repro.sh).

static void timestamp(char *buf, size_t len) {
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  struct tm tm;
  gmtime_r(&ts.tv_sec, &tm);
  snprintf(buf, len, "%02d:%02d:%02d.%03ld", tm.tm_hour, tm.tm_min, tm.tm_sec,
           ts.tv_nsec / 1000000);
}

static void
heartbeat_failed(const mongoc_apm_server_heartbeat_failed_t *event) {
  char buf[32];
  timestamp(buf, sizeof buf);
  bson_error_t error;
  mongoc_apm_server_heartbeat_failed_get_error(event, &error);
  printf("[%s] HEARTBEAT_FAILED: %s:%d | duration: %.2f ms | error: %s\n", buf,
         mongoc_apm_server_heartbeat_failed_get_host(event)->host,
         mongoc_apm_server_heartbeat_failed_get_host(event)->port,
         mongoc_apm_server_heartbeat_failed_get_duration(event) / 1000.0,
         error.message);
}

static void
heartbeat_succeeded(const mongoc_apm_server_heartbeat_succeeded_t *event) {
  int64_t duration_ms =
      mongoc_apm_server_heartbeat_succeeded_get_duration(event) / 1000;
  if (duration_ms > 100) {
    char buf[32];
    timestamp(buf, sizeof buf);
    printf("[%s] HEARTBEAT_SLOW: %s:%d | duration: %" PRId64 " ms\n", buf,
           mongoc_apm_server_heartbeat_succeeded_get_host(event)->host,
           mongoc_apm_server_heartbeat_succeeded_get_host(event)->port,
           duration_ms);
  }
}

int main(void) {
  mongoc_init();

  // Same URI options as the PHP repro.
  const char *uristr =
      "mongodb://localhost:27017,localhost:27018,localhost:27019/"
      "?readPreference=primary&retryWrites=true&retryReads=true"
      "&serverSelectionTryOnce=false&serverSelectionTimeoutMS=3000"
      "&connectTimeoutMS=1000&socketTimeoutMS=2000"
      "&socketCheckIntervalMS=1000";

  mongoc_client_t *client = mongoc_client_new(uristr);

  mongoc_apm_callbacks_t *callbacks = mongoc_apm_callbacks_new();
  mongoc_apm_set_server_heartbeat_failed_cb(callbacks, heartbeat_failed);
  mongoc_apm_set_server_heartbeat_succeeded_cb(callbacks, heartbeat_succeeded);
  mongoc_client_set_apm_callbacks(client, callbacks, NULL);
  mongoc_apm_callbacks_destroy(callbacks);

  bson_t ping = BSON_INITIALIZER;
  BSON_APPEND_INT32(&ping, "ping", 1);

  long deadline = time(NULL) + 100;
  int i = 0;
  long timer = 0;

  for (i = 0; time(NULL) < deadline; i++) {
    bson_t reply = BSON_INITIALIZER;
    bson_error_t error;

    int64_t t0 = bson_get_monotonic_time();
    bool ok = mongoc_client_command_simple(client, "admin", &ping, NULL, &reply,
                                           &error);
    int64_t us = bson_get_monotonic_time() - t0;
    double ms = us / 1000.0;

    if (!ok) {
      char buf[32];
      timestamp(buf, sizeof buf);
      printf("[%s] [%d] ERROR: %s\n", buf, i, error.message);
    } else if (ms > 100.0) {
      char buf[32];
      timestamp(buf, sizeof buf);
      printf("[%s] [%d] ping | total: %.2f ms *** SLOW\n", buf, i, ms);
    }

    bson_destroy(&reply);

    struct timespec ts = {0, 50 * 1000 * 1000}; // 50ms pause
    nanosleep(&ts, NULL);

    timer += (bson_get_monotonic_time() - t0) / 1000;
    if (timer > 1000) {
      printf(".");
      fflush(stdout);
      timer = 0;
    }
  }

  bson_destroy(&ping);
  mongoc_client_destroy(client);
  mongoc_cleanup();
  printf("DONE\n");
  return 0;
}
