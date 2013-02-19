/**
* This is a test for hatab in $package$
*/
#include "si_test.h"
#include "hatab.h"



TEST_FUNC(hatab) {
  Hatab * map;
  int     i[] = { 123, 456, 789, 1011, 1213 };
  map = hatab_newroom(NULL, 3, 3);
  TEST_NOTNULL(map);
  TEST_NOTNULL(hatab_put(map, "123", i));
  TEST_NOTNULL(hatab_put(map, "456", i+1));
  TEST_NOTNULL(hatab_put(map, "789", i+2));
  TEST_NOTNULL(hatab_put(map, "1011", i+3));
  TEST_NOTNULL(hatab_put(map, "1213", i+4));  
  TEST_PTREQ(hatab_get(map, "123"), i);
  TEST_PTREQ(hatab_get(map, "456"), i+1);
  TEST_PTREQ(hatab_get(map, "789"), i+2);
  TEST_PTREQ(hatab_get(map, "1011"), i+3);
  TEST_PTREQ(hatab_get(map, "1213"), i+4);
  TEST_NOTNULL(hatab_drop(map, "456"));
  TEST_NULL(hatab_get(map, "456"));
  TEST_NOTNULL(hatab_put(map, "456", i+1));
  TEST_NOTNULL(hatab_put(map, "456", i+1));

  TEST_NOTNULL(hatab_put(map, "1011", i+3));
  TEST_PTREQ(hatab_get(map, "1011"), i+3);
  hatab_free(map);
  TEST_DONE();
}



int main(void) {
  TEST_INIT();
  TEST_RUN(hatab);
  TEST_REPORT();
}



