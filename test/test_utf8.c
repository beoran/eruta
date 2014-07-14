/**
* This is a test for utf8 in $package$
*/
#include "si_test.h"
#include "utf8.h"


TEST_FUNC(utf8) {
  char * strlist[] = { "", "u", "µ", "無"};
  long correct[] = { 0, 117, 181, 28961 };
  int index;
  long result;
  char * aid = NULL;
  for (index = 0; index < 4; index ++) { 
    TEST_INTEQ(index, utf8_decode_length(strlist[index]));
    TEST_INTEQ(index, utf8_decode_one(strlist[index], &result));
    TEST_INTEQ(result, correct[index]);
    TEST_INTEQ(index, utf8_next(strlist[index], &aid, &result));
    if (index > 0) { 
      TEST_INTEQ(result, correct[index]);
      TEST_PTREQ((void *)aid, (void *)(strlist[index] + index));
    } else {
      TEST_INTEQ(result, 0);
      TEST_NULL((void *)aid);      
   }
  }
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(utf8);
  TEST_REPORT();
}



