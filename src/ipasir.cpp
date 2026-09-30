#include "ipasir.h"
#include "ccoursesat.h"

extern "C" {

const char *ipasir_signature () { return ccoursesat_signature (); }

void *ipasir_init () {
  CCourseSAT *coursesat = ccoursesat_init ();
  ccoursesat_set_option (coursesat, "factor", 0);
  return coursesat;
}

void ipasir_release (void *solver) {
  ccoursesat_release ((CCourseSAT *) solver);
}

void ipasir_add (void *solver, int lit) {
  ccoursesat_add ((CCourseSAT *) solver, lit);
}

void ipasir_assume (void *solver, int lit) {
  ccoursesat_assume ((CCourseSAT *) solver, lit);
}

int ipasir_solve (void *solver) {
  return ccoursesat_solve ((CCourseSAT *) solver);
}

int ipasir_val (void *solver, int lit) {
  return ccoursesat_val ((CCourseSAT *) solver, lit);
}

int ipasir_failed (void *solver, int lit) {
  return ccoursesat_failed ((CCourseSAT *) solver, lit);
}

void ipasir_set_terminate (void *solver, void *state,
                           int (*terminate) (void *state)) {
  ccoursesat_set_terminate ((CCourseSAT *) solver, state, terminate);
}

void ipasir_set_learn (void *solver, void *state, int max_length,
                       void (*learn) (void *state, int *clause)) {
  ccoursesat_set_learn ((CCourseSAT *) solver, state, max_length, learn);
}
}
