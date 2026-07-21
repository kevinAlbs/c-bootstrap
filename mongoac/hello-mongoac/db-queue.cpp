// db-queue.cpp - Process incoming requests asynchronously.
//
// Await request, start driver operation asynchronously, await next request.
//
// Uses two threads, because two things block:
//   - main thread blocks awaiting requests (await_request)
//   - worker thread blocks on the async runtime with make_progress()

#include "mongoac.hpp"

#include "db-requests.hpp"

#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <thread>
#include <vector>

namespace {

// InFlight represents one in-flight operation.
struct InFlight {
  Request req;
  mongoac::Future fut;
};

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
  {
    mongoac::Future drop = coll.drop(error);
    while (!drop.poll()) {
      runtime.makeProgress();
    }
  }

  // In-flight operations, handed from the main thread to the worker thread.
  std::vector<InFlight> inflight;
  std::mutex mtx;
  std::condition_variable cv; // Woken when work arrives or on shutdown.
  bool shutdown = false;

  // Worker thread: drive the runtime and report completions.
  std::thread worker([&] {
    mongoac::Error err; // The worker's own error, not shared with main.
    while (true) {
      runtime.makeProgress();

      std::unique_lock<std::mutex> lock(mtx);
      for (size_t i = 0; i < inflight.size();) {
        InFlight &op = inflight[i];
        if (!op.fut.poll()) {
          ++i;
          continue;
        }
        if (op.req.op == Op::Insert) {
          op.fut.getVoid(err);
          if (err.ok()) {
            printf("[OK] insert %d\n", op.req.value);
          } else {
            printf("[ERR] insert %d: %s\n", op.req.value, err.message());
          }
        } else if (op.req.op == Op::Count) {
          int32_t count = op.fut.getInt32(err);
          if (err.ok()) {
            printf("[OK] count -> %d\n", count);
          } else {
            printf("[ERR] count: %s\n", err.message());
          }
        }
        inflight.erase(inflight.begin() + i); // Future destructor runs here.
      }

      // Sleep until there is an operation to advance (or we are shutting down);
      // while operations are in flight the predicate is already true, so we keep
      // pumping the runtime without ever blocking here.
      cv.wait(lock, [&] { return !inflight.empty() || shutdown; });
      if (inflight.empty() && shutdown) {
        return;
      }
    }
  });

  // Main thread: await each request and kick off its operation asynchronously.
  while (true) {
    Request req = await_request(); // May block.
    if (req.op == Op::Shutdown) {
      {
        std::lock_guard<std::mutex> lock(mtx);
        shutdown = true;
      }
      cv.notify_one();
      break;
    }
    mongoac::Future fut = (req.op == Op::Insert)
                              ? coll.insertOne(req.arg.get(), error)
                              : coll.countDocuments(req.arg.get(), error);
    {
      std::lock_guard<std::mutex> lock(mtx);
      inflight.push_back({std::move(req), std::move(fut)});
    }
    cv.notify_one();
  }

  worker.join();
  printf("shutting down\n");
  return EXIT_SUCCESS;
}
