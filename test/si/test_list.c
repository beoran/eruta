#include <stdio.h>
#include <stddef.h>
#include "si_test.h"
#include "si_list.h"

void * cursor_search(SiCursor * cursor, void * extra) {
  char * data   = (char *) sicursor_data(cursor);
  char * tofind = (char *) extra;
  if (strcmp(data, tofind) == 0) return tofind;
  return NULL;
}

void * walker_search(void * value, void * key, void * list, void * extra) {
  char * data   = (char *) value;
  char * tofind = (char *) extra;
  printf("%s %s\n", data, tofind);
  if (strcmp(data, tofind) == 0) return tofind;
  return NULL;
}

TEST_FUNC(silist) {
  char * aid;
  char * s[] = { "Hello", "World", "This", "Is", "Me"};
  SiList * list  = silist_new(s[0]);
  TEST_NOTNULL(list);
  TEST_NOTNULL(silist_add(list, s[1]));
  TEST_NOTNULL(silist_add(list, s[2]));
  TEST_NOTNULL(silist_add(list, s[3]));
  TEST_NOTNULL(silist_add(list, s[4]));
  TEST_NOTNULL(silist_last(list));
  TEST_STREQ(silist_data(silist_last(list)), s[4]);
  aid = (char *) silist_walk(list, walker_search, "Is");
  TEST_NOTNULL(aid);
  TEST_STREQ(aid, "Is");

/*
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
    char * res = 
    (char *) sicursor_each(silist_cursor(list), cursor_search, "Me");
    TEST_NOTNULL(res);
    TEST_STREQ(res, "Me");
  }
*/
  silist_free(list);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(silist);
  TEST_REPORT();
}



