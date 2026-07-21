// db-requests.hpp - The source of incoming requests for db-queue.cpp.
//
// await_request() stands in for however an application receives work -- a network
// socket, a message queue, an HTTP handler, ... Here it just replays a fixed
// script so the example is self-contained and deterministic. The BSON argument
// for each request is built here, so the example's main never touches BSON.
//
// The scripted workload inserts integer values into a collection and
// periodically counts the documents, then ends with a Shutdown request.

#ifndef DB_REQUESTS_HPP
#define DB_REQUESTS_HPP

#include "mongoac.hpp"

#include <bson/bson.h>

#include <utility>
#include <vector>

enum class Op { Insert, Count, Shutdown };

struct Request {
  Op op;
  int value;         // The inserted value (Insert only); shown when printing.
  mongoac::Bson arg; // The insert document, or the (empty) count filter.
};

inline mongoac::Bson insert_doc(int value) {
  bson_t *doc = bson_new();
  BSON_APPEND_INT32(doc, "value", value);
  return mongoac::Bson(doc);
}

inline std::vector<Request> make_request_stream() {
  const int values[] = {50, 45, 55, 40, 60, 48, 52, 44, 56, 50,
                        30, 70, 20, 80, 10, 95};

  std::vector<Request> stream;
  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
    stream.push_back({Op::Insert, values[i], insert_doc(values[i])});
    // Once the collection is seeded, count after each further insert so the
    // count visibly grows as work is serviced.
    if (i >= 10) {
      stream.push_back({Op::Count, 0, mongoac::Bson(bson_new())});
    }
  }
  stream.push_back({Op::Shutdown, 0, mongoac::Bson()});
  return stream;
}

// Block until the next incoming request arrives. In a real application this
// would wait on a socket / queue; here it replays the scripted stream and
// returns Shutdown once the script is exhausted.
inline Request await_request() {
  static std::vector<Request> stream = make_request_stream();
  static size_t next = 0;
  if (next >= stream.size()) {
    return {Op::Shutdown, 0, mongoac::Bson()};
  }
  return std::move(stream[next++]);
}

#endif // DB_REQUESTS_HPP
