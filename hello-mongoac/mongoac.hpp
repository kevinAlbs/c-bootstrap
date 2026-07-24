// mongoac.hpp - Minimal C++ RAII wrappers over the mongoac C API.
//
// Each class owns one C handle and destroys it in its destructor; methods that
// start operations return a Future. The driver's error convention is kept
// explicit: call sites pass an Error& and check err.ok() -- nothing throws.
#ifndef MONGOAC_HPP
#define MONGOAC_HPP

#include <bson/bson.h>

#include <mongoac/client.h>
#include <mongoac/collection.h>
#include <mongoac/database.h>
#include <mongoac/error.h>
#include <mongoac/future.h>
#include <mongoac/runtime.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace mongoac {

// Owning wrapper for a bson_t returned by the driver (e.g. a query result).
class Bson {
public:
  Bson(bson_t *p = nullptr) : p_(p) {}
  ~Bson() { bson_destroy(p_); }
  Bson(Bson &&o) noexcept : p_(std::exchange(o.p_, nullptr)) {}
  Bson &operator=(Bson &&o) noexcept {
    std::swap(p_, o.p_);
    return *this;
  }
  const bson_t *get() const { return p_; }

  std::string toJSON() const {
    char *json = bson_as_relaxed_extended_json(p_, nullptr);
    std::string out = json ? json : "";
    bson_free(json);
    return out;
  }

private:
  bson_t *p_;
};

class Error {
public:
  Error() : p_(mongoac_error_new()) {}
  ~Error() { mongoac_error_destroy(p_); }
  Error(Error &&o) noexcept : p_(std::exchange(o.p_, nullptr)) {}
  Error &operator=(Error &&o) noexcept {
    std::swap(p_, o.p_);
    return *this;
  }

  bool ok() const { return mongoac_error_code(p_) == MONGOAC_ERROR_CODE_OK; }
  const char *message() const { return mongoac_error_message(p_); }
  mongoac_error_t *raw() const { return p_; } // for passing into C calls
private:
  mongoac_error_t *p_;
};

class Future {
public:
  Future(mongoac_future_t *p = nullptr) : p_(p) {}
  ~Future() { mongoac_future_destroy(p_); }
  Future(Future &&o) noexcept : p_(std::exchange(o.p_, nullptr)) {}
  Future &operator=(Future &&o) noexcept {
    std::swap(p_, o.p_);
    return *this;
  }

  bool isReady() const { return mongoac_future_is_ready(p_); }
  void getVoid(Error &e) const { mongoac_future_get_void(p_, e.raw()); }
  int32_t getInt32(Error &e) const {
    return mongoac_future_get_int32(p_, e.raw());
  }
  Bson getBson(Error &e) const {
    return Bson(mongoac_future_get_bson(p_, e.raw()));
  }

  // The underlying handle, for passing into runtime block-on calls.
  const mongoac_future_t *get() const { return p_; }

private:
  mongoac_future_t *p_;
};

// Borrowed handle: owned by the Client, so its destructor does NOT destroy it.
class Runtime {
public:
  explicit Runtime(mongoac_runtime_t *p) : p_(p) {}

  // Drive the runtime one turn, advancing any runnable tasks -- including
  // background tasks with no FutureT (e.g. killCursors on cursor destroy,
  // endSessions on client destroy). Returns immediately; does not park.
  bool makeProgress() const { return mongoac_runtime_make_progress(p_); }

  // Drive the runtime, parking (no spin) until `future` resolves.
  void blockOn(const Future &future, Error &e) const {
    mongoac_runtime_block_on(p_, future.get(), e.raw());
  }

  // Drive the runtime, parking until *any* of `futures` resolves; returns the
  // handle that became ready (or nullptr). The C call returns a pointer *into*
  // `futures`, so dereference it to recover the ready handle.
  const mongoac_future_t *blockOnAny(const std::vector<const mongoac_future_t *> &futures,
                                     Error &e) const {
    const mongoac_future_t *const *slot =
        mongoac_runtime_block_on_any(p_, futures.data(), futures.size(), e.raw());
    return slot ? *slot : nullptr;
  }

  // Same, but returns after `ms` even if none completed (then Error is set to a
  // timeout and the result is nullptr). Lets the caller re-check its work set
  // periodically so newly-arrived operations aren't starved behind a slow one.
  const mongoac_future_t *blockOnAnyWithTimeout(
      const std::vector<const mongoac_future_t *> &futures, uint64_t ms, Error &e) const {
    const mongoac_future_t *const *slot = mongoac_runtime_block_on_any_with_timeout(
        p_, futures.data(), futures.size(), ms, e.raw());
    return slot ? *slot : nullptr;
  }

  // Park until new work is spawned or a stop is requested (bounded).
  bool waitWithTimeout(uint64_t ms) const {
    return mongoac_runtime_wait_with_timeout(p_, ms);
  }

  // Ask any blocked wait/worker to wake so it can observe shutdown.
  void requestStop() const { mongoac_runtime_request_stop(p_); }
  bool stopRequested() const { return mongoac_runtime_stop_requested(p_); }

private:
  mongoac_runtime_t *p_;
};

class Collection {
public:
  Collection(mongoac_collection_t *p = nullptr) : p_(p) {}
  ~Collection() { mongoac_collection_destroy(p_); }
  Collection(Collection &&o) noexcept : p_(std::exchange(o.p_, nullptr)) {}
  Collection &operator=(Collection &&o) noexcept {
    std::swap(p_, o.p_);
    return *this;
  }

  Future insertOne(const bson_t *document, Error &e) const {
    return Future(mongoac_collection_insert_one_async(p_, document, e.raw()));
  }
  Future countDocuments(const bson_t *filter, Error &e) const {
    return Future(
        mongoac_collection_count_documents_async(p_, filter, e.raw()));
  }
  Future drop(Error &e) const {
    return Future(mongoac_collection_drop_async(p_, nullptr, e.raw()));
  }

private:
  mongoac_collection_t *p_;
};

class Database {
public:
  Database(mongoac_database_t *p = nullptr) : p_(p) {}
  ~Database() { mongoac_database_destroy(p_); }
  Database(Database &&o) noexcept : p_(std::exchange(o.p_, nullptr)) {}
  Database &operator=(Database &&o) noexcept {
    std::swap(p_, o.p_);
    return *this;
  }

  Collection getCollection(const char *name, Error &e) const {
    return Collection(mongoac_database_get_collection(p_, name, e.raw()));
  }

private:
  mongoac_database_t *p_;
};

class Client {
public:
  Client(const char *uri, Error &e) : p_(mongoac_client_new(uri, e.raw())) {}
  ~Client() { mongoac_client_destroy(p_); }
  Client(Client &&o) noexcept : p_(std::exchange(o.p_, nullptr)) {}
  Client &operator=(Client &&o) noexcept {
    std::swap(p_, o.p_);
    return *this;
  }

  // Borrowed -- lifetime tied to this Client.
  Runtime getRuntime() const { return Runtime(mongoac_client_get_runtime(p_)); }
  Database getDatabase(const char *name, Error &e) const {
    return Database(mongoac_client_get_database(p_, name, nullptr, e.raw()));
  }

private:
  mongoac_client_t *p_;
};

} // namespace mongoac

#endif // MONGOAC_HPP
