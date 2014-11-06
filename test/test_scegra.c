/**
* This is a test for scegra in $package$
*/
#include "si_test.h"
#include "scegra.h"



TEST_FUNC(scegra) {
  /*
  ScegraStyle style;
  float x, y, z;
  scegra_init();
  scegrastyle_initempty(&style);
  TEST_INTEQ(123, scegra_make_box(123, bevec(100,200), bevec(21,43), bevec(4, 7), style));
  TEST_INTEQ(321, scegra_make_text(321, bevec(200, 300), "Hello", style));
  TEST_INTEQ(501, scegra_make_image(501, bevec(200, 300), 77, style));
  scegra_speed(123, &x, &y);
  TEST_FLOATEQ(0.0, x);
  TEST_FLOATEQ(0.0, y);
  
  scegra_speed(123, &x, &y);
  TEST_FLOATEQ(0.0, x);
  TEST_FLOATEQ(0.0, y);
  
  
  scegra_done();
  */
  TEST_DONE();
}


int main(void) {
  TEST_INIT();  
  TEST_RUN(scegra);
  TEST_REPORT();
}



