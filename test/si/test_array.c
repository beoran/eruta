/**
* This is a test for array in si
*/
#include "si_test.h"
#include "si_array.h"


TEST_FUNC(si_array) {
  SiArray * array = siarray_newptr(64);
  siarray_free(array);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(si_array);
  TEST_REPORT();
}



