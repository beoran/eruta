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
  fprintf(stderr, "sizeof RaVar : %d\n", sizeof(RaVar)); 
  TEST_DONE();
}


int main(int argc, char * argv[]) {
  int result;
  
  TEST_INIT(); 
  TEST_RUN(raku_mem);
  TEST_RUN(raku_string);
  TEST_REPORT();
}



