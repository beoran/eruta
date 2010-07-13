#ifndef _TEST_H_
#define _TEST_H_

struct Test_ ; 
typedef struct Test_ Test;

struct Test_ {
  int count;
  int ok;
  int failed;
};

/* 
*  The header file carries it's own implementation around,
*  which is fine for simple test programs. 
*/   
#ifndef TESTING_NO_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


/** Initializes the test. */
Test * test_init(Test * test) {
  test->failed = test->ok = test->count = 0;
  return test; 
}

/** Adds one OK test.*/
Test * test_ok(Test * test) {
  test->count ++;
  test->ok    ++;
  return test;  
}


/** Fails one test with variable arguments. */
Test * test_fail_va(Test * test, char * fmt, va_list ap) {
  test->count ++;
  test->failed++;
  fprintf(stderr, "Test %d failed: ", test->count);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  return test;
} 


/** Fails one test. */
Test * test_fail(Test * test, char * fmt, ...) {
  va_list  ap;  
  va_start(ap, fmt);
  test_fail_va(test, fmt, ap);
  va_end(ap); 
  return test;
}


/** 
Reports the results of the tests. Retuns nonzero if there were errors, zero if no errors, so it can be called in main as return test_report(&test);
*/
int test_report(Test * test) {
  if (test->ok == test->count) {
    fprintf(stderr, "SUCCESS: %d/%d \n", test->ok, test->count);
    return 0;
  } else {
    fprintf(stderr, "FAILED: %d/%d \n", test->ok, test->count);
    return 1;
  }
}

/** Tests if an assertion is true with variable arguments. */
Test * test_assert_va(Test * test, int assert, char * fmt, va_list ap) {
  if(assert) { // if the assertion is true 
    test_ok(test);
  } else {
    test_fail_va(test, fmt, ap);
  }
}


/** Tests if a certain asertion is true, failing if it false. */
Test * test_assert(Test * test, int assert, char * fmt, ...) {
  va_list  ap;  
  va_start(ap, fmt);
  test_assert_va(test, assert, fmt, ap);
  va_end(ap); 
  return test;  
}


/** Tests if a pointer is null, faling the test if it's not so. */
Test * test_null(Test * test, void * ptr, char * explain) {
  return test_assert(test, ptr == NULL, 
              "Pointer should be null: %p; %s",  ptr, explain); 
}

/** Tests if a pointer is not null, faling the test if it is NULL. */
Test * test_notnull(Test * test, void * ptr, char * explain) {
  return test_assert(test, ptr != NULL, 
  "Pointer should be not null: %p; %s",  ptr, explain);
}

/** Tests if two strings are equal according to strcmp. */
Test * test_streq(Test * test, char * s1, char * s2, char * explain) {
  return test_assert(test, strcmp(s1, s2) == 0, 
  "Strings should be equal: %s %s; %s ", s1, s2, explain);
}

/** Tests if two memory areas are equal according to memcmp. */
Test * test_memeq(Test * test, void * m1, void * m2, size_t n, 
                  char * explain) {
  return test_assert(test, memcmp(m1, m2, n) == 0, 
  "Memory should be equal: %p %p; %s ", m1, m2, explain);
}

/* Tests if two integers are equal */
Test * test_inteq(Test * test, int i1, int i2, char * explain) {
  return test_assert(test, i1 == i2, 
  "Integers should be equal: %d %d; %s ", i1, i2, explain);
}

/* Macros to help with calling tested functions.
* Important notice: these macros assume that a Test * t exists.    
*/  
#define TEST_CALL(CALL)       CALL,#CALL
#define TEST_ASSERT(CALL)     test_assert(_t, TEST_CALL(CALL) )
#define TEST_NULL(CALL)       test_null(_t, TEST_CALL(CALL) )
#define TEST_NOTNULL(CALL)    test_notnull(_t, TEST_CALL(CALL) )
#define TEST_STREQ(STR, CALL) test_streq(_t, STR, TEST_CALL(CALL))
#define TEST_MEMEQ(MEM, CALL) test_memeq(_t, MEM, TEST_CALL(CALL))
#define TEST_INTEQ(INT, CALL) test_inteq(_t, INT, TEST_CALL(CALL))

/* Other utility macros. They are syntactic, so you may dislike them. */
#define TEST_FUNC(NAME)       Test * test_##NAME(Test * _t) 
#define TEST_DONE()           return _t
#define TEST_RUN(NAME)        test_##NAME(_t)
#define TEST_INIT()           Test _st ;  Test * _t ; _t = &_st ; test_init(_t)
#define TEST_REPORT()         return test_report(_t);




#endif




#endif
