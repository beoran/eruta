/**
* This is a test for ui in $package$
*/
#include "si_test.h"
#include "eruta.h"
#include "image.h"
#include "ui.h"

#include <stdarg.h>

int test_va(int last, ...) {
  int res = 0;  double d = 0.0; char * str = NULL;
  va_list args;
  va_start(args, last);
  va_list_scan(args, "ifs", &res, &d, &str);
  va_end(args);
  printf("%d %f %s\n", res, d, str);
  return res;
}


TEST_FUNC(ui) {
  TEST_INTEQ(1375, test_va(0, 1375, 12.34, "Aha!"));
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(ui);
  TEST_REPORT(); 
}


