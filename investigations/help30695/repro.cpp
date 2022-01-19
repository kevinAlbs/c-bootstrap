#include <bson/bson.h>

#include <array>
#include <thread>
#include <set>
#include <vector>
#include <iostream>

struct oid_compare {
   bool
   operator() (bson_oid_t left, bson_oid_t right) const noexcept
   {
      return bson_oid_compare_unsafe (&left, &right) < 0;
   }
};

static bool
test_once ()
{
   std::set<bson_oid_t, oid_compare> unique_oids;

   const int n_threads = 16;
   constexpr int n_per_thread = 1'000;

   std::vector<std::thread> threads;
   std::vector<std::array<bson_oid_t, n_per_thread>> thread_oid_arrs;
   thread_oid_arrs.resize (n_threads);

   threads.resize (n_threads);
   int thread_n = 0;
   for (auto &t : threads) {
      t = std::thread{[&, thread_n] {
         std::array<bson_oid_t, n_per_thread> &oid_array =
            thread_oid_arrs[thread_n];
         for (auto &oid : oid_array) {
            auto ctx = bson_context_new (BSON_CONTEXT_NONE);
            bson_oid_init (&oid, ctx);
            bson_context_destroy (ctx);
         }
      }};
      ++thread_n;
   }

   for (auto &t : threads) {
      t.join ();
   }

   for (auto &vec : thread_oid_arrs) {
      unique_oids.insert (vec.cbegin (), vec.cend ());
   }

   const int n_expected = n_threads * n_per_thread;

   if (n_expected == unique_oids.size ()) {
      return true;
   }
   std::cout << "Generated " << unique_oids.size () << " unique OIDs\n";
   std::cout << "(Expected " << n_expected << " OIDs)\n";
   return false;
}

int
main ()
{
   const int n_tries = 1000;
   for (int i = 0; i < n_tries; ++i) {
      std::cout << "Attempt #" << i + 1 << '\n';
      if (!test_once ()) {
         return 1;
      }
   }
   return 0;
}
