/**
* This is a test for tmatrix in $package$
*/
#include "si_test.h"


/* Get Tmatrix interface, generate for integer type */
#define TEMPLATE_T      int
#define TEMPLATE_NAME   Intmat
#define TEMPLATE_PREFIX intmat_
#include "tmatrix.h"

/* Get implementation for iteger type. */
#define TEMPLATE_IMPLEMENT
#define TEMPLATE_T      int
#define TEMPLATE_NAME   Intmat
#define TEMPLATE_PREFIX intmat_

#include "tmatrix.h"

#undef TEMPLATE_T
#undef TEMPLATE_NAME
#undef TEMPLATE_PREFIX
#undef TEMPLATE_IMPLEMENT

/* Get Tmatrix interface generate for pointer type */
#define TEMPLATE_T      void*
#define TEMPLATE_NAME   Ptrmat
#define TEMPLATE_PREFIX ptrmat_
#include "tmatrix.h"

/* Get implementation for pointer type. */
#define TEMPLATE_IMPLEMENT
#define TEMPLATE_T      void*
#define TEMPLATE_NAME   Ptrmat
#define TEMPLATE_PREFIX ptrmat_
#include "tmatrix.h"

#undef TEMPLATE_T
#undef TEMPLATE_NAME
#undef TEMPLATE_PREFIX
#undef TEMPLATE_IMPLEMENT



TEST_FUNC(tmatrix_int) {
  Intmat matrix_struct; 
  Intmat * matrix = &matrix_struct;
  int get;
  TEST_NOTNULL(intmat_init(matrix, 5, 7));
  TEST_FALSE(intmat_put(matrix, 3, 4, 123));
  TEST_FALSE(intmat_put(matrix, 0, 0, 654));
  TEST_TRUE(intmat_put(matrix, 5, 4, 321));
  TEST_TRUE(intmat_put(matrix, 1, 7, 321));
  TEST_TRUE(intmat_put(matrix, 10, 20, 456));
  TEST_INTEQ(123, intmat_getraw(matrix, 3, 4));
  TEST_INTEQ(654, intmat_getraw(matrix, 0, 0));
  TEST_FALSE(intmat_get(matrix, 3, 4, &get));
  TEST_INTEQ(123, get);  
  TEST_FALSE(intmat_get(matrix, 0, 0, &get));
  TEST_INTEQ(654, get);
  TEST_TRUE(intmat_get(matrix, 5, 4, &get));
  TEST_TRUE(intmat_get(matrix, 1, 7, &get));
  TEST_TRUE(intmat_get(matrix, 10, 20, &get));
  TEST_NULL(intmat_free(matrix));
  TEST_DONE();
}

TEST_FUNC(tmatrix_ptr) {
  Ptrmat matrix_struct; 
  Ptrmat * matrix = &matrix_struct;
  int i1 = 123;
  int i2 = 234;
  int i3 = 354;
  int i4 = 456;
  void * get;
  void * p1 = &i1;
  void * p2 = &i2;
  void * p3 = &i3;
  void * p4 = &i4;
  
  TEST_NOTNULL(ptrmat_init(matrix, 5, 7));
  TEST_FALSE(ptrmat_put(matrix, 3, 4, p1));
  TEST_FALSE(ptrmat_put(matrix, 0, 0, p2));
  TEST_TRUE(ptrmat_put(matrix, 5, 4, p3));
  TEST_TRUE(ptrmat_put(matrix, 1, 7, p3));
  TEST_TRUE(ptrmat_put(matrix, 10, 20, p4));
  TEST_PTREQ(p1, ptrmat_getraw(matrix, 3, 4));
  TEST_PTREQ(p2, ptrmat_getraw(matrix, 0, 0));
  TEST_FALSE(ptrmat_get(matrix, 3, 4, &get));
  TEST_PTREQ(p1, get);  
  TEST_FALSE(ptrmat_get(matrix, 0, 0, &get));
  TEST_PTREQ(p2, get);
  TEST_TRUE(ptrmat_get(matrix, 5, 4, &get));
  TEST_TRUE(ptrmat_get(matrix, 1, 7, &get));
  TEST_TRUE(ptrmat_get(matrix, 10, 20, &get));
  TEST_NULL(ptrmat_free(matrix));
  TEST_DONE();
}



int main(void) {
  TEST_INIT();
  TEST_RUN(tmatrix_int);
  TEST_RUN(tmatrix_ptr);
  TEST_REPORT();
}



