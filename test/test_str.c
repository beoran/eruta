/**
* This is a test for str in $package$
*/
#include "si_test.h"
#include "str.h"


TEST_FUNC(str) {

  /** Test the cstr_simplematch function */
  TEST_ASSERT(cstr_simplematch("$a", 'x'));
  TEST_ASSERT(cstr_simplematch("$a", '0'));
  TEST_ASSERT(cstr_simplematch("$A", 'x'));
  TEST_ASSERT(!cstr_simplematch("$A", '0'));
  TEST_ASSERT(cstr_simplematch(".", '\n'));
  TEST_ASSERT(cstr_simplematch("@\n\r", '\n'));
  TEST_ASSERT(!cstr_simplematch("!@\r\n", '\n'));
  TEST_ASSERT(!cstr_simplematch("", '\n'));
  TEST_ASSERT(!cstr_simplematch(NULL, '\n'));
  TEST_ASSERT(cstr_simplematch("@0123456789", '5'));
  TEST_ASSERT(!cstr_simplematch("@0123456789", ' '));
  
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(str);
  TEST_REPORT();
}



