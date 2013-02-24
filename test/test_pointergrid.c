/**
* This is a test for pointergrid in $package$
*/
#include "si_test.h"
#include "pointergrid.h"


TEST_FUNC(pointergrid) {
  PointerGrid   matrix_struct; 
  PointerGrid * matrix = &matrix_struct;
  int i1 = 123;
  int i2 = 234;
  int i3 = 354;
  int i4 = 456;
  void * get;
  void * p1 = &i1;
  void * p2 = &i2;
  void * p3 = &i3;
  void * p4 = &i4;
  
  TEST_NOTNULL(pointergrid_init(matrix, 5, 7));
  TEST_FALSE(pointergrid_put(matrix, 3, 4, p1));
  TEST_FALSE(pointergrid_put(matrix, 0, 0, p2));
  TEST_TRUE(pointergrid_put(matrix, 5, 4, p3));
  TEST_TRUE(pointergrid_put(matrix, 1, 7, p3));
  TEST_TRUE(pointergrid_put(matrix, 10, 20, p4));
  TEST_PTREQ(p1, pointergrid_getraw(matrix, 3, 4));
  TEST_PTREQ(p2, pointergrid_getraw(matrix, 0, 0));
  TEST_FALSE(pointergrid_get(matrix, 3, 4, &get));
  TEST_PTREQ(p1, get);  
  TEST_FALSE(pointergrid_get(matrix, 0, 0, &get));
  TEST_PTREQ(p2, get);
  TEST_TRUE(pointergrid_get(matrix, 5, 4, &get));
  TEST_TRUE(pointergrid_get(matrix, 1, 7, &get));
  TEST_TRUE(pointergrid_get(matrix, 10, 20, &get));
  TEST_NOTNULL(pointergrid_done(matrix, NULL));
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(pointergrid);
  TEST_REPORT();
}



