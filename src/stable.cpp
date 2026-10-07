#ifdef PROFILE_MODE

#include "internal.hpp"

namespace CourseSAT {

bool Internal::propagate_stable () {
  assert (stable);
  START (propstable);
  bool res = propagate ();
  STOP (propstable);
  return res;
}

void Internal::analyze_stable () {
  assert (stable);
  START (analyzestable);
  analyze ();
  STOP (analyzestable);
}

int Internal::decide_stable () {
  assert (stable);
  return decide ();
}

}; // namespace CourseSAT

#else
int stable_if_not_profile_mode_dummy;
#endif
