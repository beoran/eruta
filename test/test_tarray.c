/**
* This is a test for tarray in tarray.h
*/



/* Get TArray interface, generate for integer type */
#define TEMPLATE_T      int
#define TEMPLATE_ZERO   0
#define TEMPLATE_NAME   Intarr
#define TEMPLATE_PREFIX intarr_
#include "tarray.h"

/* Get implementation for integer type. */
#define TEMPLATE_IMPLEMENT
#define TEMPLATE_T      int
#define TEMPLATE_NAME   Intarr
#define TEMPLATE_PREFIX intarr_
#define TEMPLATE_ZERO   0
#include "tarray.h"

/* Get Tarray interface for pointer type */
#define TEMPLATE_T      void*
#define TEMPLATE_NAME   Ptrarr
#define TEMPLATE_PREFIX ptrarr_
#define TEMPLATE_ZERO   NULL
#include "tarray.h"
#include "si_test.h"

/* Get implementation for pointer type. */
#define TEMPLATE_IMPLEMENT
#define TEMPLATE_T      void*
#define TEMPLATE_NAME   Ptrarr
#define TEMPLATE_PREFIX ptrarr_
#define TEMPLATE_ZERO   NULL
#include "tarray.h"




TEST_FUNC(tarray_int) {
  Intarr array_struct; 
  Intarr * array = &array_struct;
  int get;       
  TEST_NOTNULL(intarr_init(array, 7));
  TEST_NULL(intarr_put(array, 9, 123));
  TEST_FALSE(intarr_put(array, -1, 654));
  TEST_TRUE(intarr_put(array, 5, 321));
  TEST_TRUE(intarr_put(array, 1, 231));
  TEST_TRUE(intarr_put(array, 0, 456));
  TEST_INTEQ(321, intarr_getraw(array, 5));
  TEST_INTEQ(231, intarr_getraw(array, 1));
  TEST_INTEQ(456, intarr_getraw(array, 0));
  
  TEST_INTEQ(0, intarr_get(array, 1, &get));
  TEST_INTEQ(231, get);  
  TEST_INTEQ(0, intarr_get(array, 0, &get));
  TEST_INTEQ(456, get);
  TEST_INTEQ(-1, intarr_get(array, 9, &get));
  TEST_INTEQ(-1, intarr_get(array, 7, &get));
  TEST_INTEQ(-1, intarr_get(array, -1, &get));
    
  TEST_NOTNULL(intarr_done(array));
  TEST_DONE();
}

TEST_FUNC(tarray_ptr) {
  Ptrarr array_struct; 
  Ptrarr * array = &array_struct;
  int i1 = 123;
  int i2 = 234;
  int i3 = 354;
  int i4 = 456;
  void * get;
  void * p1 = &i1;
  void * p2 = &i2;
  void * p3 = &i3;
  void * p4 = &i4;
  
  TEST_NOTNULL(ptrarr_init(array, 7));
  TEST_NULL(ptrarr_put(array, 8, p1));
  TEST_NULL(ptrarr_put(array, -1, p2));
  TEST_NOTNULL(ptrarr_put(array, 5, p1));
  TEST_NOTNULL(ptrarr_put(array, 1, p2));
  TEST_NOTNULL(ptrarr_put(array, 3, p3));
  TEST_PTREQ(p1, ptrarr_getraw(array, 5));
  TEST_PTREQ(p2, ptrarr_getraw(array, 1));
  TEST_INTEQ(0, ptrarr_get(array, 5, &get));
  TEST_PTREQ(p1, get);  
  TEST_INTEQ(0, ptrarr_get(array, 1, &get));
  TEST_PTREQ(p2, get);
  TEST_INTEQ(-1, ptrarr_get(array, -1, &get));
  TEST_INTEQ(-1, ptrarr_get(array, 7, &get));
  TEST_INTEQ(-1, ptrarr_get(array, 20, &get));
  TEST_NOTNULL(ptrarr_done(array));
  TEST_DONE();
}



int main(void) {
  TEST_INIT();
  TEST_RUN(tarray_int);
  TEST_RUN(tarray_ptr);
  TEST_REPORT();
}


