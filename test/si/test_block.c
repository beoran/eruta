/**
* This is a test for block in si
*/
#include "si_test.h"
#include "si_block.h"


TEST_FUNC(si_block) {
  char * s[] = { "Hello", "World", "This", "Is", "Me"};
  SiBlock * b = siblock_newempty(sizeof(char *));
  void * mem  = si_malloc(10000);
  printf("Stack addr of s, s[2] and b: %p, %p, %p, %p\n", s, s[2], b, mem);
  printf("%s %s %s\n", s[0], s[1], s[2]);
  TEST_NOTNULL(b);
  TEST_INTEQ(siblock_elementsize(b), sizeof(char *));
  TEST_INTEQ(siblock_size(b), 0);
  TEST_NULL(siblock_get(b, 10));
  TEST_NULL(siblock_get(b, 1));  
  TEST_NOTNULL(siblock_add(b, s + 0));
  TEST_NOTNULL(siblock_add(b, s + 1));
  TEST_INTEQ(siblock_size(b), 2);
  TEST_NOTNULL(siblock_get(b, 1));
  TEST_NOTNULL(siblock_get(b, 0));
  TEST_STREQ(*((char **)siblock_get(b, 1)), s[1]);
  TEST_STREQ(*((char **)siblock_get(b, 0)), s[0]);
  TEST_NOTNULL(siblock_set(b, 301, s + 3));
  TEST_NOTNULL(siblock_get(b, 301));
  TEST_INTEQ(siblock_size(b), 302);
  TEST_STREQ(*((char **)siblock_get(b, 301)), s[3]);
  TEST_NOTNULL(siblock_set(b, 401, s + 4));
  TEST_INTEQ(siblock_size(b), 402);
  TEST_NOTNULL(siblock_get(b, 401));
  TEST_STREQ(*((char **)siblock_get(b, 401)), s[4]);
  si_free(mem);
  siblock_free(b);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(si_block);
  TEST_REPORT();
}



