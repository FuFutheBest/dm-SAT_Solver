#include "coursesat.hpp"

#include <cstdlib>
#include <cstring>

namespace CourseSAT {

struct Wrapper : Learner, Terminator {

  Solver *solver;
  struct {
    void *state;
    int (*function) (void *);
  } terminator;

  struct {
    void *state;
    int max_length;
    int *begin_clause, *end_clause, *capacity_clause;
    void (*function) (void *, int *);
  } learner;

  bool terminate () {
    if (!terminator.function)
      return false;
    return terminator.function (terminator.state);
  }

  bool learning (int size) {
    if (!learner.function)
      return false;
    return size <= learner.max_length;
  }

  void learn (int lit) {
    if (learner.end_clause == learner.capacity_clause) {
      size_t count = learner.end_clause - learner.begin_clause;
      size_t size = count ? 2 * count : 1;
      learner.begin_clause =
          (int *) realloc (learner.begin_clause, size * sizeof (int));
      learner.end_clause = learner.begin_clause + count;
      learner.capacity_clause = learner.begin_clause + size;
    }
    *learner.end_clause++ = lit;
    if (lit)
      return;
    learner.function (learner.state, learner.begin_clause);
    learner.end_clause = learner.begin_clause;
  }

  Wrapper () : solver (new Solver ()) {
    memset (&terminator, 0, sizeof terminator);
    memset (&learner, 0, sizeof learner);
  }

  ~Wrapper () {
    terminator.function = 0;
    if (learner.begin_clause)
      free (learner.begin_clause);
    delete solver;
  }
};

} // namespace CourseSAT

using namespace CourseSAT;

extern "C" {

#include "ccoursesat.h"

const char *ccoursesat_signature (void) { return Solver::signature (); }

CCourseSAT *ccoursesat_init (void) { return (CCourseSAT *) new Wrapper (); }

void ccoursesat_release (CCourseSAT *wrapper) {
  delete (Wrapper *) wrapper;
}

void ccoursesat_constrain (CCourseSAT *wrapper, int lit) {
  ((Wrapper *) wrapper)->solver->constrain (lit);
}

int ccoursesat_constraint_failed (CCourseSAT *wrapper) {
  return ((Wrapper *) wrapper)->solver->constraint_failed ();
}

void ccoursesat_set_option (CCourseSAT *wrapper, const char *name,
                            int val) {
  ((Wrapper *) wrapper)->solver->set (name, val);
}

void ccoursesat_limit (CCourseSAT *wrapper, const char *name, int val) {
  ((Wrapper *) wrapper)->solver->limit (name, val);
}

int ccoursesat_get_option (CCourseSAT *wrapper, const char *name) {
  return ((Wrapper *) wrapper)->solver->get (name);
}

void ccoursesat_add (CCourseSAT *wrapper, int lit) {
  ((Wrapper *) wrapper)->solver->add (lit);
}

void ccoursesat_assume (CCourseSAT *wrapper, int lit) {
  ((Wrapper *) wrapper)->solver->assume (lit);
}

int ccoursesat_solve (CCourseSAT *wrapper) {
  return ((Wrapper *) wrapper)->solver->solve ();
}

int ccoursesat_simplify (CCourseSAT *wrapper) {
  return ((Wrapper *) wrapper)->solver->simplify ();
}

int ccoursesat_val (CCourseSAT *wrapper, int lit) {
  return ((Wrapper *) wrapper)->solver->val (lit);
}

int ccoursesat_failed (CCourseSAT *wrapper, int lit) {
  return ((Wrapper *) wrapper)->solver->failed (lit);
}

void ccoursesat_print_statistics (CCourseSAT *wrapper) {
  ((Wrapper *) wrapper)->solver->statistics ();
}

void ccoursesat_terminate (CCourseSAT *wrapper) {
  ((Wrapper *) wrapper)->solver->terminate ();
}

int64_t ccoursesat_active (CCourseSAT *wrapper) {
  return ((Wrapper *) wrapper)->solver->active ();
}

int64_t ccoursesat_irredundant (CCourseSAT *wrapper) {
  return ((Wrapper *) wrapper)->solver->irredundant ();
}

int ccoursesat_fixed (CCourseSAT *wrapper, int lit) {
  return ((Wrapper *) wrapper)->solver->fixed (lit);
}

void ccoursesat_set_terminate (CCourseSAT *ptr, void *state,
                               int (*terminate) (void *)) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->terminator.state = state;
  wrapper->terminator.function = terminate;
  if (terminate)
    wrapper->solver->connect_terminator (wrapper);
  else
    wrapper->solver->disconnect_terminator ();
}

void ccoursesat_set_learn (CCourseSAT *ptr, void *state, int max_length,
                           void (*learn) (void *state, int *clause)) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->learner.state = state;
  wrapper->learner.max_length = max_length;
  wrapper->learner.function = learn;
  if (learn)
    wrapper->solver->connect_learner (wrapper);
  else
    wrapper->solver->disconnect_learner ();
}

void ccoursesat_freeze (CCourseSAT *ptr, int lit) {
  ((Wrapper *) ptr)->solver->freeze (lit);
}

void ccoursesat_melt (CCourseSAT *ptr, int lit) {
  ((Wrapper *) ptr)->solver->melt (lit);
}

int ccoursesat_frozen (CCourseSAT *ptr, int lit) {
  return ((Wrapper *) ptr)->solver->frozen (lit);
}

int ccoursesat_trace_proof (CCourseSAT *ptr, FILE *file, const char *path) {
  return ((Wrapper *) ptr)->solver->trace_proof (file, path);
}

void ccoursesat_close_proof (CCourseSAT *ptr) {
  ((Wrapper *) ptr)->solver->close_proof_trace ();
}

void ccoursesat_conclude (CCourseSAT *ptr) {
  ((Wrapper *) ptr)->solver->conclude ();
}

int ccoursesat_vars (CCourseSAT *ptr) {
  return ((Wrapper *) ptr)->solver->vars ();
}

int ccoursesat_declare_more_variables (CCourseSAT *ptr,
                                       int number_of_vars) {
  return ((Wrapper *) ptr)->solver->declare_more_variables (number_of_vars);
}

int ccoursesat_declare_one_more_variable (CCourseSAT *ptr) {
  return ((Wrapper *) ptr)->solver->declare_one_more_variable ();
}

void ccoursesat_phase (CCourseSAT *wrapper, int lit) {
  ((Wrapper *) wrapper)->solver->phase (lit);
}

void ccoursesat_unphase (CCourseSAT *wrapper, int lit) {
  ((Wrapper *) wrapper)->solver->unphase (lit);
}
}
