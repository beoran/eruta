/**
* This is a test for silut in $package$
*/
#include "si_test.h"
#include "silut.h"


TEST_FUNC(silut) {  
  Silut * aid;
  static Silut lut[] = {
    { 0, "zero"},
    { 2, "two" },
    { 1, "one" },
    SILUT_DONE
  };
  
  aid = silut_lsearchi(lut, 55);
  TEST_NULL(aid);
  aid = silut_lsearchcstr(lut, "banana");
  TEST_NULL(aid);
  aid = silut_lsearchi(lut, 2);
  TEST_NOTNULL(aid);
  TEST_INTEQ(aid->integer, 2);
  TEST_STREQ(aid->string, "two");
  aid = silut_lsearchcstr(lut, "two");
  TEST_NOTNULL(aid);
  TEST_INTEQ(aid->integer, 2);
  TEST_STREQ(aid->string, "two");
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(silut);
  TEST_REPORT();
}



