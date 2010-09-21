#include "testing.h"
#include "raku.h"
#include <stdio.h>

TEST_FUNC(raku_mem) {
  void * mem;
  mem = ra_mem_allot(123);
  ra_mem_free(mem);  
  TEST_DONE();
}


TEST_FUNC(raku_string) {
  RaString s1, s2, s3, s4;
  fprintf(stderr, "sizeof RaVar : %d\n", sizeof(RaVar)); 
  s1 = ra_string_fromc("Hello!");
  ra_string_puts(s1);
  fprintf(stderr, "%p %p\n", s1, s1->data); 
  RA_OBJECT_TOSS(s1);
  TEST_DONE();
}


int main(int argc, char * argv[]) {
  int result;
  
  TEST_INIT(); 
  TEST_RUN(raku_mem);
  TEST_RUN(raku_string);
  TEST_REPORT();
}



