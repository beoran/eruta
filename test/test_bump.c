/**
* This is a test for bump in $package$
*/
#include "si_test.h"
#include "bump.h"



TEST_FUNC(bump) {
  BumpVec v1, v2, vr;
  v1 = bumpvec(1.0, 2.0);
  v2 = bumpvec(3.0, 4.0);
  TEST_DOUBLEEQ(v1.x, 1.0);
  TEST_DOUBLEEQ(v1.y, 2.0);
  TEST_DOUBLEEQ(v2.x, 3.0);
  TEST_DOUBLEEQ(v2.y, 4.0);
  vr = bumpvec_add(v1, v2);
  TEST_DOUBLEEQ(vr.x, 4.0);
  TEST_DOUBLEEQ(vr.y, 6.0);
  vr = bumpvec_sub(v2, v1);
  TEST_DOUBLEEQ(vr.x, 2.0);
  TEST_DOUBLEEQ(vr.y, 2.0);  
  vr = bumpvec_mul(v1, 3.0);
  TEST_DOUBLEEQ(vr.x, 3.0);
  TEST_DOUBLEEQ(vr.y, 6.0);
  vr = bumpvec_div(v1, 10.0);
  TEST_DOUBLEEQ(vr.x, 0.1);
  TEST_DOUBLEEQ(vr.y, 0.2);  
  vr = bumpvec_div(v1, 0.0);
  TEST_DOUBLEEQ(vr.x, 0);
  TEST_DOUBLEEQ(vr.y, 0);
  
  
  
  
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(bump);
  TEST_REPORT();
}



