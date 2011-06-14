#include "re/mem.h"
#include "re/test.h"

TEST_FUNC(re_malloc) {
  void * aid = re_malloc(10);
  TEST_NOTNULL(aid);
  TEST_NULL(re_free(aid));  
  TEST_DONE();
}

TEST_FUNC(re_memcpy) {
  char src[10] = "banana";
  char dst[10];
  char * res = re_memcpy(dst, src, 7);
  TEST_NOTNULL(res);
  TEST_INTEQ(res[0], 'b');
  TEST_DONE();
}

TEST_FUNC(re_realloc) {
  char * aid = re_malloc(10);
  char * new = NULL;  
  TEST_NOTNULL(aid);
  TEST_NULL(new);
  aid[2] = 10;
  TEST_INTEQ(aid[2], 10);
  new    = re_realloc(aid, 20);
  TEST_NOTNULL(new);
  TEST_INTEQ(new[2], 10);
  TEST_NULL(re_free(new));
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(re_malloc);
  TEST_RUN(re_memcpy);
  TEST_RUN(re_realloc);
  TEST_REPORT();
}




