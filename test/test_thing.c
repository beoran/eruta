/**
* This is a test for thing in $package$
*/
#include "si_test.h"
#include "thing.h"


TEST_FUNC(thing) {
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(thing);
  TEST_REPORT();
}



