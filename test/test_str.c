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
  USTR * ustr;
  USTRListNode * node;
  char * data[] = { "one", "two", "three", "four" };
  char * zero   = "zero";
  USTRList list;
  /** Test the cstr_simplematch function */
  TEST_NOTNULL(ustrlist_init(&list));
  
  
  for(index = 0; index < 4; index ++) {
    TEST_NOTNULL(ustrlist_addcstr(&list, data[index]));
  }
  TEST_NOTNULL(ustrlist_shiftcstr(&list, zero));
  
  for(node = ustrlist_head(&list); node; node = ustrlistnode_next(node)) {
    printf("%s\n", ustr_c(ustrlistnode_ustr(node)));
  }
  TEST_NOTNULL(ustrlist_droplast(&list));
  for(node = ustrlist_head(&list); node; node = ustrlistnode_next(node)) {
    printf("%s\n", ustr_c(ustrlistnode_ustr(node)));
  }
  ustr = ustrlist_joinwithcstr(&list, " , ");
  printf("%s\n", ustr_c(ustr));
  TEST_NOTNULL(ustrlist_shiftustr(&list, ustr));
  ustr_free(ustr);
  for(node = ustrlist_head(&list); node; node = ustrlistnode_next(node)) {
    printf("%s\n", ustr_c(ustrlistnode_ustr(node)));
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



