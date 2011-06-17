#include "si_mem.h"
#include "si_test.h"

TEST_FUNC(si_malloc) {
  void * aid = si_malloc(10);
  TEST_NOTNULL(aid);
  TEST_NULL(si_free(aid));  
  TEST_DONE();
}


TEST_FUNC(si_memcpy) {
  char src[10] = "banana";
  char dst[10];
  char * res = si_memcpy(dst, src, 7);
  TEST_NOTNULL(res);
  TEST_INTEQ(res[0], 'b');
  TEST_INTEQ(res[5], 'a');
  TEST_DONE();
}

TEST_FUNC(si_copyalloc) {
  char src[10] = "banana";
  char *   res = si_copyalloc(5, src, 3);
  TEST_NOTNULL(res);
  TEST_INTEQ(res[0], 'b');
  TEST_INTEQ(res[1], 'a');
  TEST_INTEQ(res[2], 'n');
  TEST_INTEQ(res[3],  0 );
  TEST_DONE();
}


TEST_FUNC(si_smemcpy) {
  char src[8] = "banana";
  char dst[10] = { 0, 1, 2, 3, 4, 5, 6 , 7 ,  8, 9 };
  char * res = si_smemcpy(dst, 14,  7, src, 8, 1, 77);
  TEST_NOTNULL(res);
  TEST_INTEQ(res[0], 0);
  TEST_INTEQ(res[6], 6);
  TEST_INTEQ(res[7], 'a');
  TEST_INTEQ(res[8], 'n');
  TEST_INTEQ(res[9], 'a');
  TEST_DONE();
}


TEST_FUNC(si_realloc) {
  char * aid = si_malloc(10);
  char * new = NULL;  
  TEST_NOTNULL(aid);
  TEST_NULL(new);
  aid[2] = 10;
  TEST_INTEQ(aid[2], 10);
  new    = si_realloc(aid, 20);
  TEST_NOTNULL(new);
  TEST_INTEQ(new[2], 10);
  TEST_NULL(si_free(new));
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(si_malloc);
  TEST_RUN(si_memcpy);
  TEST_RUN(si_smemcpy);
  TEST_RUN(si_copyalloc);  
  TEST_RUN(si_realloc);
  TEST_REPORT();
}




