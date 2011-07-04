/**
* This is a test for table in si
*/
#include "si_test.h"
#include "si_table.h"

TEST_FUNC(si_table) {
  SiTable * tab = sitable_new(NULL);
  char * k1 = "hello";
  char * k2 = "world";
  char * k3 = "!";
  char * v1 = "olleh";
  char * v2 = "dlrow";
  char * v3 = "?";
  TEST_NOTNULL(tab);
  TEST_NULL(sitable_get(tab, k1));
  TEST_NOTNULL(sitable_set(tab, k1, v1));
  TEST_NOTNULL(sitable_set(tab, k2, v2));
  TEST_NOTNULL(sitable_set(tab, k3, v3));
  TEST_NOTNULL(sitable_get(tab, k1));
  TEST_NOTNULL(sitable_get(tab, k3));
  TEST_NOTNULL(sitable_get(tab, k2));
  TEST_STREQ(sitable_get(tab, k1), v1);
  TEST_STREQ(sitable_get(tab, k2), v2);
  TEST_STREQ(sitable_get(tab, k3), v3);
  TEST_STREQ(sitable_drop(tab, k3), v3);
  TEST_NULL(sitable_get(tab, k3));
  TEST_NOTNULL(sitable_set(tab, k3, v3));
  TEST_NOTNULL(sitable_get(tab, k3));
  TEST_NOTNULL(sitable_grow(tab, 1024));
  TEST_NOTNULL(sitable_get(tab, k3));
  TEST_STREQ(sitable_drop(tab, k3), v3);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(si_table);
  TEST_REPORT();
}



