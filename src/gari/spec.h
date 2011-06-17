#ifndef _SPEC_H_
#define _SPEC_H_

struct Suite_ ; 
typedef struct Suite_ Suite;

struct Spec_ ; 
typedef struct Spec_ Spec;

struct Spec_ {
  char * it;
  int    ok;
  char * should;
};


#define SUITE_SPECS 1024 

struct Suite_ {
  Spec specs[SUITE_SPECS];
  int count;
  int ok;
  int failed;
  char * describe;
};




/* 
*  The header file carries it's own implementation around,
*  which is fine for simple specs. 
*/   
#ifndef TESTING_NO_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/** Initializes the suite. */
Suite * suite_init(Suite * suite, char * describe) {
  suite->count    = 0;
  suite->ok       = 0;
  suite->failed   = 0;
  suite->describe = describe;
  return suite;
}

/** Returns the adddress of the spec at the given index. */
Spec * suite_get(Suite * suite, int index) {
  return suite->specs + index;
}

/** Returns the next empty spec in the suite. */
Spec * suite_next_spec(Suite * suite) {
  Spec * spec;
  spec = suite_get(suite, suite->count);
  suite->count++;
  return spec;
}

/** Initializes the spec. */
Spec * spec_init(Spec * spec, char * it) {
  spec->ok = 1;
  spec->it = it;
  return spec; 
}

/** Calls should on spec. */
Spec * spec_should(Spec * spec, int value, char * should) {
  spec->ok     = value;
  spec->should = should;
  return spec;
}


/** 
* Reports the results of the suite.
*/
int suite_report(Suite * suite) {
  int index; Spec * spec;
  for (index = 0; index < suite->count; index++) { 
    spec = suite_get(suite, index);
    if (spec->ok) {
      suite->ok++; 
    } else {
      suite->failed++; 
      fprintf(stderr, "FAILED: %s %s:\n%s \n", suite->describe, spec->it, spec->should);
    }   
  }     
  if (suite->ok == suite->count) {
    fprintf(stderr, "SUCCESS: %d/%d \n", suite->ok, suite->count);
    return 0;
  } else {
    fprintf(stderr, "FAILED: %d/%d \n", suite->ok, suite->count);
    return 1;
  }
}

/**
* Macros for the spec. They are syntactic, but it saves you 
* from having to use another preprocessor.
*/  
#define describe(SUITENAME)                    \
          int main(void) {                     \
            Suite _ss, *_s; Spec * _p; int _b; \
            _s = &_ss;                         \
            suite_init(_s, #SUITENAME);
        
#define done                          \
            return suite_report(_s);  \
          }
 
#define it(SPECNAME)                  \
            _p = suite_next_spec(_s); \
            spec_init(_p, #SPECNAME); 
             
 
#define should(VALUE)           spec_should(_p, VALUE, #VALUE) 
#define should_be(VAR, VALUE)   spec_should(_p, VAR == VALUE, #VAR "!=" #VALUE)
#define should_be_null(VAR)     spec_should(_p, VAR == NULL, #VAR "!=NULL")
#define should_not_be_null(VAR) spec_should(_p, VAR != NULL, #VAR "==NULL")



#endif




#endif
