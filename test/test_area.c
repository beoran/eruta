/**
* This is a test for area in $package$
*/
#include "si_test.h"
#include "area.h"


TEST_FUNC(area) {
  Area * area;
  area = area_new();
  TEST_NOTNULL(area);
  TEST_NULL(area_free(area));  
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(area);
  TEST_REPORT();
}



