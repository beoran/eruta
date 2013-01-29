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

TEST_FUNC(strlist) {
  int index;
  BadListNode  * bnode;
  USTRListNode * node;
  
  USTRList list;
  /** Test the cstr_simplematch function */
  TEST_NOTNULL(ustrlist_init(&list));
  
  char * data[] = { "one", "two", "three", "four" };
  for(index = 0; index < 4; index ++) {
    TEST_NOTNULL(ustrlist_addcstr(&list, data[index]));
  }
  for(bnode = ustrlist_head(&list); bnode; bnode = badlistnode_next(bnode)) {
    node = badlistnode_ustrlistnode(bnode);
    printf("%s", ustr_c(ustrlistnode_ustr(node)));
  }
  
  
  TEST_NOTNULL(ustrlist_done(&list));
  TEST_DONE();
}



int main(void) {
  TEST_INIT();
  TEST_RUN(str);
  TEST_RUN(strlist);
  TEST_REPORT();
}



