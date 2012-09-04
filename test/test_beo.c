/**
* This is a test for beo in $package$
*/
#include "si_test.h"
#include "beo.h"


TEST_FUNC(beo) {
/* Testing function for Beo */
  Beo       * beo;
  BeoValue  * v;
  v = beovalue_newstr("123.456");
  TEST_NOTNULL(v);
  TEST_LONGEQ(beovalue_getlong(v, NULL), 123) ;    
  TEST_DOUBLEEQ(beovalue_getdouble(v, NULL), 123.456);    
  beo_unref(v, NULL);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(beo);
  TEST_REPORT();
}



