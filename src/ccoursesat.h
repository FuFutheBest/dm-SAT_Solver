#ifndef _ccoursesat_h_INCLUDED
#define _ccoursesat_h_INCLUDED

/*------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/*------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdio.h>

// C wrapper for CourseSAT's C++ API following IPASIR.

typedef struct CCourseSAT CCourseSAT;

const char *ccoursesat_signature (void);
CCourseSAT *ccoursesat_init (void);
void ccoursesat_release (CCourseSAT *);

void ccoursesat_add (CCourseSAT *, int lit);
void ccoursesat_assume (CCourseSAT *, int lit);
int ccoursesat_solve (CCourseSAT *);
int ccoursesat_val (CCourseSAT *, int lit);
int ccoursesat_failed (CCourseSAT *, int lit);

void ccoursesat_set_terminate (CCourseSAT *, void *state,
                               int (*terminate) (void *state));

void ccoursesat_set_learn (CCourseSAT *, void *state, int max_length,
                           void (*learn) (void *state, int *clause));

/*------------------------------------------------------------------------*/

// Non-IPASIR conformant 'C' functions.

void ccoursesat_constrain (CCourseSAT *, int lit);
int ccoursesat_constraint_failed (CCourseSAT *);
void ccoursesat_set_option (CCourseSAT *, const char *name, int val);
void ccoursesat_limit (CCourseSAT *, const char *name, int limit);
int ccoursesat_get_option (CCourseSAT *, const char *name);
void ccoursesat_print_statistics (CCourseSAT *);
int64_t ccoursesat_active (CCourseSAT *);
int64_t ccoursesat_irredundant (CCourseSAT *);
int ccoursesat_fixed (CCourseSAT *, int lit);
int ccoursesat_trace_proof (CCourseSAT *, FILE *, const char *);
void ccoursesat_close_proof (CCourseSAT *);
void ccoursesat_conclude (CCourseSAT *);
void ccoursesat_terminate (CCourseSAT *);
void ccoursesat_freeze (CCourseSAT *, int lit);
int ccoursesat_frozen (CCourseSAT *, int lit);
void ccoursesat_melt (CCourseSAT *, int lit);
int ccoursesat_simplify (CCourseSAT *);
int ccoursesat_vars (CCourseSAT *);
int ccoursesat_declare_more_variables (CCourseSAT *, int number_of_vars);
int ccoursesat_declare_one_more_variable (CCourseSAT *);
void ccoursesat_phase (CCourseSAT *wrapper, int lit);
void ccoursesat_unphase (CCourseSAT *wrapper, int lit);

/*------------------------------------------------------------------------*/

// Support legacy names used before moving to more IPASIR conforming names.

#define ccoursesat_reset ccoursesat_release
#define ccoursesat_sat ccoursesat_solve
#define ccoursesat_deref ccoursesat_val

/*------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
/*------------------------------------------------------------------------*/

#endif
