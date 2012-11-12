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
  
  static Silut lut_s[] = {
    { 1, "one" },
    { 2, "two" },
    { 0, "zero"},
    SILUT_DONE
  };
  
  static Silut lut_i[] = {
    { 0, "zero"},
    { 1, "one" },
    { 2, "two" },    
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
  aid = silut_bsearchcstr(lut_s, "two");
  TEST_NOTNULL(aid);
  TEST_INTEQ(aid->integer, 2);
  TEST_STREQ(aid->string, "two");
  aid = silut_bsearchcstr(lut_s, "banana");
  TEST_NULL(aid);
  
  aid = silut_bsearchint(lut_i, 1);
  TEST_NOTNULL(aid);
  TEST_STREQ(aid->string, "one");
  TEST_INTEQ(aid->integer, 1);
    
  
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(silut);
  TEST_REPORT();
}



