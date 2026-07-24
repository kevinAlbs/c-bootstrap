// db-queue.cpp - Process incoming requests asynchronously.
//
// Uses two threads, because two things block:
//   - the main thread blocks awaiting requests (await_request).
//   - a worker thread drives the runtime and reports results.

#include "db-requests.hpp"
#include "mongoac.hpp"

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <thread>
#include <vector>

namespace {

// InFlight represents one in-flight task.
struct InFlight {
  Request req;
  mongoac::Future fut;
};

// handle_result handles the result of a completed task.
void handle_result(const Request &req, const mongoac::Future &fut,
                   mongoac::Error &err) {
  if (req.op == Op::Insert) {
    fut.getVoid(err);
    if (err.ok()) {
      printf("[OK] insert %d\n", req.value);
    } else {
      printf("[ERR] insert %d: %s\n", req.value, err.message());
    }
  } else if (req.op == Op::Count) {
    int32_t count = fut.getInt32(err);
    if (err.ok()) {
      printf("[OK] count -> %d\n", count);
    } else {
      printf("[ERR] count: %s\n", err.message());
    }
  }
}

} // namespace

int main(void) {
  mongoac::Error error;

  mongoac::Client client("mongodb://localhost:27017", error);
  if (!error.ok()) {
    fprintf(stderr, "client creation failed: %s\n", error.message());
    return EXIT_FAILURE;
  }
  mongoac::Runtime runtime = client.getRuntime();
  mongoac::Database db = client.getDatabase("demo", error);
  mongoac::Collection coll = db.getCollection("numbers", error);

  // Start from a clean collection so the reported counts are deterministic.
  runtime.blockOn(coll.drop(error), error);

  // In-flight tasks, handed from the main thread to the worker thread.
  std::vector<InFlight> inflight;
  std::mutex mtx;
  std::condition_variable cv; // Signals the worker when work arrives / on stop.
  bool shutdown_requested = false;

  // Worker thread: drive the runtime and report completions until shutdown is
  // requested and all in-flight tasks have drained.
  std::thread worker([&] {
    mongoac::Error err; // The worker's own error, not shared with main.
    while (true) {
      // Copy handles to in-flight tasks:
      std::vector<const mongoac_future_t *> pending;
      {
        std::unique_lock<std::mutex> lock(mtx);
        // Wake on new work, shutdown, or after 10ms to drive background tasks
        cv.wait_for(lock, std::chrono::milliseconds(10),
                    [&] { return !inflight.empty() || shutdown_requested; });
        if (inflight.empty() && shutdown_requested) {
          return;
        }
        for (const InFlight &op : inflight) {
          pending.push_back(op.fut.get());
        }
      }

      // Drive pending futures and background tasks:
      if (!pending.empty()) {
        // Use a 10ms timeout to avoid starving incoming requests.
        runtime.blockOnAnyWithTimeout(pending, 10, err);
      } else {
        // Drive background tasks (e.g. `killCursors` on cursor destroy):
        runtime.makeProgress();
      }

      // Move completed tasks to handle results:
      std::vector<InFlight> completed;
      {
        std::lock_guard<std::mutex> lock(mtx);
        for (size_t i = 0; i < inflight.size();) {
          if (!inflight[i].fut.isReady()) {
            ++i;
            continue;
          }
          completed.push_back(std::move(inflight[i]));
          inflight.erase(inflight.begin() + i);
        }
      }

      // Handle results outside of lock:
      for (const InFlight &op : completed) {
        handle_result(op.req, op.fut, err);
      }
    }
  });

  // Main thread: await each request and kick off its task asynchronously.
  while (true) {
    Request req = await_request(); // Blocks.
    if (req.op == Op::Shutdown) {
      {
        std::lock_guard<std::mutex> lock(mtx);
        shutdown_requested = true;
      }
      cv.notify_one(); // Wake the worker if it is idle in cv.wait().
      break;
    }
    mongoac::Future fut = (req.op == Op::Insert)
                              ? coll.insertOne(req.arg.get(), error)
                              : coll.countDocuments(req.arg.get(), error);
    {
      std::lock_guard<std::mutex> lock(mtx);
      inflight.push_back({std::move(req), std::move(fut)});
    }
    cv.notify_one(); // Wake the worker if it is idle in cv.wait().
  }

  worker.join();
  printf("shutting down\n");
  return EXIT_SUCCESS;
}
