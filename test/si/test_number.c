/**
* This is a test for number in si
*/
#include "si_test.h"
#include "si_number.h"




TEST_FUNC(si_number) {
  double da = 10.0, db = 20.0, dm;
  int ia = 10, ib = 20, im;
  SI_MIN_T(double, da, db, dm);
  TEST_ASSERT(dm == da);
  SI_MIN_T(int, ia, ib, im);
  TEST_INTEQ(im, ia);
  TEST_DONE();
}

int main(void) {
  TEST_INIT();
  TEST_RUN(si_number);
  TEST_REPORT();
}



