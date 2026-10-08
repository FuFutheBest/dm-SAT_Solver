// g++ -std=c++11 -O1 -g -fsanitize=address,undefined -Isrc \
//   test/reap-regression.cpp src/reap.cpp -o /tmp/reap-test
#include "reap.hpp"
#include <cassert>
#include <climits>
#include <random>
#include <set>

int main () {
  Reap reap;
  std::multiset<unsigned> reference;
  std::mt19937 random (42);
  unsigned last = 0;
  for (unsigned step = 0; step < 500000; ++step) {
    if (random () % 17 == 0) {
      reap.clear ();
      reference.clear ();
      last = 0;
    } else if (!reference.empty () && random () % 2) {
      last = *reference.begin ();
      assert (reap.pop () == last);
      reference.erase (reference.begin ());
    } else {
      const unsigned value = last + random () % 1024;
      reap.push (value);
      reference.insert (value);
    }
    assert (reap.size () == reference.size ());
    assert (reap.empty () == reference.empty ());
  }
  reap.clear ();
  for (unsigned bit = 0; bit < 32; ++bit)
    reap.push (1u << bit);
  reap.push (UINT_MAX);
  for (unsigned bit = 0; bit < 32; ++bit)
    assert (reap.pop () == (1u << bit));
  assert (reap.pop () == UINT_MAX);
  reap.clear ();
  reap.clear ();
  reap.push (0);
  reap.push (0);
  assert (!reap.pop ());
  assert (!reap.pop ());
  assert (reap.empty ());
}
