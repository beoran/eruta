#include <stdio.h>
#include <stddef.h>
#include "si_test.h"
#include "si_list.h"

TEST_FUNC(silist) {
  SiList * list  = silist_new("Hello");
  TEST_NOTNULL(list);
  TEST_NOTNULL(silist_add(list, "World"));
  TEST_NOTNULL(silist_add(list, "this"));
  TEST_NOTNULL(silist_add(list, "is"));
  TEST_NOTNULL(silist_add(list, "me"));
  TEST_NOTNULL(silist_last(list));
  TEST_STREQ(silist_data(silist_last(list)), "me");
  silist_free(list);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(silist);
  TEST_REPORT();
}



