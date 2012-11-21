/**
* This is a test for inli in $package$
*/
#include "si_test.h"
#include "inli.h"

typedef struct Element_ Element;
typedef struct ElementList_ ElementList;

struct Element_ {
  int try;
};

struct ElementList_ {
  struct Element_ data;
  Inli list;
};

TEST_FUNC(inli) {
  ElementList l1, l2, l3, l4, l5;
  TEST_NOTNULL(inli_init(&l1.list));
  TEST_NOTNULL(inli_init(&l2.list));
  TEST_NOTNULL(inli_init(&l3.list));
  TEST_NOTNULL(inli_init(&l4.list));
  TEST_NOTNULL(inli_init(&l5.list));
  TEST_NULL(inli_next(&l1.list));
  TEST_NULL(inli_next(&l2.list));
  TEST_NULL(inli_prev(&l1.list));
  TEST_NULL(inli_prev(&l2.list));
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(inli);
  TEST_REPORT();
}



