#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// HELP-103593: does server selection block on a down secondary's hello
// even when the primary is known and the op targets the primary?
// Run against a 3-node RS; SIGSTOP a secondary mid-run.
//
// Mirrors help103593.py so the two can be compared directly:
//  - Pings the cluster every ~50ms.
//  - Prints "." about once per second as progress.
//  - Prints server type changes (SERVER_CHANGED), heartbeat started /
//    succeeded / failed events (to spot a hung heartbeat), plus ping errors
//    and slow pings (>1s) since those are what an application would see.
//
// Set MONGODB_URI to override the default URI.

static void timestamp(char *buf, size_t len) {
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  struct tm tm;
  gmtime_r(&ts.tv_sec, &tm);
  snprintf(buf, len, "%02d:%02d:%02d.%03ld", tm.tm_hour, tm.tm_min, tm.tm_sec,
           ts.tv_nsec / 1000000);
}

static void server_changed(const mongoc_apm_server_changed_t *event) {
  const char *prev = mongoc_server_description_type(
      mongoc_apm_server_changed_get_previous_description(event));
  const char *next = mongoc_server_description_type(
      mongoc_apm_server_changed_get_new_description(event));
  if (strcmp(prev, next) == 0) {
    return;
  }
  const mongoc_host_list_t *host = mongoc_apm_server_changed_get_host(event);
  char buf[32];
  timestamp(buf, sizeof buf);
  // Leading newline so output doesn't follow a progress "." line.
  printf("[%s] SERVER_CHANGED: %s:%d %s -> %s\n", buf, host->host, host->port,
         prev, next);
  fflush(stdout);
}

static void
heartbeat_started(const mongoc_apm_server_heartbeat_started_t *event) {
  const mongoc_host_list_t *host =
      mongoc_apm_server_heartbeat_started_get_host(event);
  char buf[32];
  timestamp(buf, sizeof buf);
  printf("[%s] HEARTBEAT_STARTED: %s:%d\n", buf, host->host, host->port);
  fflush(stdout);
}

static void
heartbeat_succeeded(const mongoc_apm_server_heartbeat_succeeded_t *event) {
  const mongoc_host_list_t *host =
      mongoc_apm_server_heartbeat_succeeded_get_host(event);
  char buf[32];
  timestamp(buf, sizeof buf);
  printf("[%s] HEARTBEAT_SUCCEEDED: %s:%d duration=%.0fms\n", buf, host->host,
         host->port,
         mongoc_apm_server_heartbeat_succeeded_get_duration(event) / 1000.0);
  fflush(stdout);
}

static void
heartbeat_failed(const mongoc_apm_server_heartbeat_failed_t *event) {
  const mongoc_host_list_t *host =
      mongoc_apm_server_heartbeat_failed_get_host(event);
  bson_error_t error;
  mongoc_apm_server_heartbeat_failed_get_error(event, &error);
  char buf[32];
  timestamp(buf, sizeof buf);
  printf("[%s] HEARTBEAT_FAILED: %s:%d duration=%.0fms error=%s\n", buf,
         host->host, host->port,
         mongoc_apm_server_heartbeat_failed_get_duration(event) / 1000.0,
         error.message);
  fflush(stdout);
}

int main(void) {
  setvbuf(stdout, NULL, _IOLBF, 0); // keep output ordered when redirected
  mongoc_init();

  // Same URI options as help103593.py.
  const char *uristr = getenv("MONGODB_URI");
  if (!uristr || !*uristr) {
    uristr = "mongodb://localhost:27017,localhost:27018,localhost:27019/"
             "?readPreference=primary&retryWrites=true&retryReads=true"
             "&serverSelectionTryOnce=false&serverSelectionTimeoutMS=3000"
             "&connectTimeoutMS=1000&socketTimeoutMS=2000"
             "&heartbeatFrequencyMS=2000&socketCheckIntervalMS=1000";
  }

  bson_error_t uri_error;
  mongoc_uri_t *uri = mongoc_uri_new_with_error(uristr, &uri_error);
  if (!uri) {
    fprintf(stderr, "Invalid MONGODB_URI: %s\n", uri_error.message);
    return 1;
  }
  mongoc_client_t *client = mongoc_client_new_from_uri(uri);
  mongoc_uri_destroy(uri);

  mongoc_apm_callbacks_t *callbacks = mongoc_apm_callbacks_new();
  mongoc_apm_set_server_changed_cb(callbacks, server_changed);
  mongoc_apm_set_server_heartbeat_started_cb(callbacks, heartbeat_started);
  mongoc_apm_set_server_heartbeat_succeeded_cb(callbacks, heartbeat_succeeded);
  mongoc_apm_set_server_heartbeat_failed_cb(callbacks, heartbeat_failed);
  mongoc_client_set_apm_callbacks(client, callbacks, NULL);
  mongoc_apm_callbacks_destroy(callbacks);

  bson_t ping = BSON_INITIALIZER;
  BSON_APPEND_INT32(&ping, "ping", 1);

  long timer = 0;

  // Runs until interrupted (Ctrl+C).
  for (;;) {
    bson_t reply = BSON_INITIALIZER;
    bson_error_t error;

    int64_t t0 = bson_get_monotonic_time();
    bool ok = mongoc_client_command_simple(client, "admin", &ping, NULL, &reply,
                                           &error);
    double ms = (bson_get_monotonic_time() - t0) / 1000.0;

    if (!ok) {
      char buf[32];
      timestamp(buf, sizeof buf);
      printf("\n[%s] ERROR: %s\n", buf, error.message);
    } else if (ms > 1000.0) {
      char buf[32];
      timestamp(buf, sizeof buf);
      printf("\n[%s] ping | total: %.2f ms *** SLOW\n", buf, ms);
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
  printf("\nDONE\n");
  return 0;
}
