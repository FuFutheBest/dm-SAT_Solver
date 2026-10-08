// Standalone regression: g++ -std=c++11 -O2 -Isrc test/heap-regression.cpp
// -o /tmp/heap-test
#include "heap.hpp"
#include <algorithm>
#include <random>
#include <vector>

struct Priority {
  const std::vector<int> *scores;
  bool operator() (unsigned a, unsigned b) const {
    if ((*scores)[a] != (*scores)[b])
      return (*scores)[a] < (*scores)[b];
    return a > b;
  }
};

int main () {
  const unsigned n = 257;
  std::vector<int> scores (n, 0);
  std::vector<bool> present (n, false);
  Priority less = {&scores};
  CourseSAT::heap<Priority> heap (less);
  std::mt19937 random (42);
  for (unsigned step = 0; step < 200000; ++step) {
    const unsigned x = random () % n;
    if (!present[x]) {
      heap.push_back (x);
      present[x] = true;
    } else {
      // Exercise increasing and decreasing priorities, including ties.
      scores[x] = (int) (random () % 31);
      heap.update (x);
    }
    unsigned best = n;
    size_t count = 0;
    for (unsigned i = 0; i < n; ++i) {
      assert (heap.contains (i) == present[i]);
      if (!present[i])
        continue;
      ++count;
      if (best == n || less (best, i))
        best = i;
    }
    assert (heap.size () == count);
    assert (heap.front () == best);
    if (random () % 3 == 0) {
      assert (heap.pop_front () == best);
      present[best] = false;
    }
    if (step % 1000 == 999) {
      while (!heap.empty ()) {
        const unsigned top = heap.pop_front ();
        present[top] = false;
        if (!heap.empty ())
          assert (!less (top, heap.front ()));
      }
      heap.clear ();
    }
  }
}
