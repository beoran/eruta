/**
* This is a test for flags in $package$
*/
#include <stdlib.h>
#include "si_test.h"
#include "flags.h"


TEST_FUNC(flags) {
  int flags = 0;
  TEST_INTEQ( 1, flags_set(&flags, 1));
  TEST_TRUE(flags_get(flags, 1));
  TEST_FALSE(flags_get(flags, 2));
  TEST_INTEQ( 3, flags_put(&flags, 2, 1));
  TEST_TRUE(flags_get(flags, 2));
  TEST_INTEQ( 1, flags_put(&flags, 2, 0));
  TEST_FALSE(flags_get(flags, 2));
  TEST_DONE();
}



int main(void) {
  TEST_INIT();
  TEST_RUN(flags);
  TEST_REPORT();
}



