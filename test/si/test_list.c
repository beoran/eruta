#include <stdio.h>
#include <stddef.h>
#include "si_test.h"
#include "si_list.h"

void * cursor_search(SiCursor * cursor, void * extra) {
  char * data   = (char *) sicursor_data(cursor);
  char * tofind = (char *) extra;
  if (strcmp(data, tofind) == 1) return tofind;
  return NULL;
}

TEST_FUNC(silist) {
  SiList * list  = silist_new("Hello");
  TEST_NOTNULL(list);
  TEST_NOTNULL(silist_add(list, "World"));
  TEST_NOTNULL(silist_add(list, "this"));
  TEST_NOTNULL(silist_add(list, "is"));
  TEST_NOTNULL(silist_add(list, "me"));
  TEST_NOTNULL(silist_last(list));
  TEST_STREQ(silist_data(silist_last(list)), "me");
  { 
    SiCursor * index = silist_cursor(list);
    int okindex = 0;
    TEST_NOTNULL(index);
    while(index) {
      TEST_INTEQ(okindex, sicursor_index(index));
      puts((char *)sicursor_data(index));
      index = sicursor_next(index);
      okindex++;
    }
  }   
  { 
    char * res = (char *)sicursor_each(silist_cursor(list), cursor_search, "is");
    TEST_NOTNULL(res);
    TEST_STREQ(res, "me");
  }
  silist_free(list);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(silist);
  TEST_REPORT();
}



