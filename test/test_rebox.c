/**
* This is a test for rebox in $package$
*/
#include "si_test.h"
#include "rebox.h"


TEST_FUNC(rebox) {
  Rebox b1, b2, b3, b4;
  Point p1, p2, p3, p4;
  float f1  = 10.1f, f2 = 20.2f, f3 = 30.3f, f4 = 40.4f;
  float f6  = 50.5f, f7 = 50.5f, f8 = 60.6f, f9 = 70.7f;
  
  b1        = rebox_new(f1, f2, f3, f4);
  p1        = rebox_at(&b1);
  TEST_FLOATEQ(f1, p1.x);
  TEST_FLOATEQ(f2, p1.y);
  TEST_FLOATEQ(f1, rebox_x(&b1));
  TEST_FLOATEQ(f2, rebox_y(&b1));
  TEST_FLOATEQ(f1 + (f3/2.0f), rebox_center_x(&b1));
  TEST_FLOATEQ(f2 + (f4/2.0f), rebox_center_y(&b1));
  rebox_x_(&b1, f6);
  rebox_y_(&b1, f7);
  TEST_FLOATEQ(f6, rebox_x(&b1));
  TEST_FLOATEQ(f7, rebox_y(&b1));
  
  
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(rebox);
  TEST_REPORT();
}



