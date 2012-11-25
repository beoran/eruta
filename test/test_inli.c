/**
* This is a test for inli in $package$
*/
#include "si_test.h"
#include <stddef.h>
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
  
  /* Test initialization. */
  TEST_NOTNULL(inli_init(&l1.list));
  TEST_NOTNULL(inli_init(&l2.list));
  TEST_NOTNULL(inli_init(&l3.list));
  TEST_NOTNULL(inli_init(&l4.list));
  TEST_NOTNULL(inli_init(&l5.list));
  
  /* Test proper initialization. */
  TEST_NULL(inli_next(&l1.list));
  TEST_NULL(inli_next(&l2.list));
  TEST_NULL(inli_prev(&l1.list));
  TEST_NULL(inli_prev(&l2.list));
  
  /* Test add to list. */
  /* l2 l3 */
  TEST_NOTNULL(inli_add(&l2.list, &l3.list));  
  /* l1 l2 l3 */
  TEST_NOTNULL(inli_unshift(&l2.list, &l1.list));
  /* l1 l2 l3 l 4 */
  TEST_NOTNULL(inli_push(&l1.list, &l4.list));
  
  
  /* Test next and prev. */
  TEST_PTREQ(inli_next(&l1.list), &l2.list);
  TEST_PTREQ(inli_prev(&l2.list), &l1.list);
  TEST_PTREQ(inli_next(&l2.list), &l3.list);
  TEST_PTREQ(inli_prev(&l3.list), &l2.list);
  TEST_PTREQ(inli_next(&l3.list), &l4.list);
  TEST_PTREQ(inli_prev(&l4.list), &l3.list);
  TEST_PTREQ(inli_data(&l1.list, offsetof(ElementList, list)), &l1);
  TEST_PTREQ(INLI_DATA(&l1.list, ElementList, list), &l1);
  TEST_PTREQ(inli_data(&l2.list, offsetof(ElementList, list)), &l2);
  TEST_PTREQ(INLI_DATA(&l2.list, ElementList, list), &l2);
  TEST_PTREQ(inli_data(&l3.list, offsetof(ElementList, list)), &l3);
  TEST_PTREQ(INLI_DATA(&l3.list, ElementList, list), &l3);
  TEST_PTREQ(inli_data(&l4.list, offsetof(ElementList, list)), &l4);
  TEST_PTREQ(INLI_DATA(&l4.list, ElementList, list), &l4);
  TEST_PTREQ(INLI_LISTDATA(&l4.list, ElementList), &l4);
  
  /* Test first. */
  TEST_PTREQ(inli_first(&l4.list), &l1.list);
  TEST_PTREQ(inli_first(&l3.list), &l1.list);
  TEST_PTREQ(inli_first(&l2.list), &l1.list);
  TEST_PTREQ(inli_first(&l1.list), &l1.list);
  
  /* Test last. */
  TEST_PTREQ(inli_last(&l1.list), &l4.list);
  TEST_PTREQ(inli_last(&l2.list), &l4.list);
  TEST_PTREQ(inli_last(&l3.list), &l4.list);
  TEST_PTREQ(inli_last(&l4.list), &l4.list);
  
  /* Test remove. */
  TEST_PTREQ(inli_remove(&l2.list), &l2.list);
  TEST_NULL(inli_next(&l2.list));
  TEST_NULL(inli_prev(&l2.list));

  TEST_PTREQ(inli_next(&l1.list), &l3.list);
  TEST_PTREQ(inli_prev(&l3.list), &l1.list);
  TEST_PTREQ(inli_next(&l3.list), &l4.list);
  TEST_PTREQ(inli_prev(&l4.list), &l3.list);
  
  /* Test remove at end. */  
  TEST_PTREQ(inli_pop(&l1.list), &l4.list);
  TEST_NULL(inli_next(&l4.list));
  TEST_NULL(inli_prev(&l4.list));
  TEST_NULL(inli_next(&l3.list));
  
 
  /* Test remove at beginning. */  
  TEST_PTREQ(inli_shift(&l1.list), &l1.list);
  TEST_NULL(inli_next(&l1.list));
  TEST_NULL(inli_prev(&l1.list));
  /** l3 since l2 was already deleted from the list. */
  TEST_NULL(inli_prev(&l3.list));  

  
  /* NULL resistance. */
  TEST_NULL(inli_init(NULL));
  TEST_NULL(inli_next(NULL));
  TEST_NULL(inli_prev(NULL));
  TEST_NULL(inli_add(NULL, NULL));
  TEST_NULL(inli_unshift(NULL, NULL));
  TEST_NULL(inli_push(NULL, NULL));
  TEST_NULL(inli_pop(NULL));
  TEST_NULL(inli_shift(NULL));  
  TEST_NULL(inli_data(NULL, 123));
  TEST_NULL(INLI_LISTDATA(NULL, ElementList));
  
  
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(inli);
  TEST_REPORT();
}



