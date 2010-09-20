#include "testing.h"
#include "raku.h"
#include <stdio.h>

TEST_FUNC(raku_string) {
  fprintf(stderr, "sizeof RaVar : %d\n", sizeof(RaVar)); 
  TEST_DONE();
}


int main(int argc, char * argv[]) {
  int result;
  
  TEST_INIT(); 
  TEST_RUN(raku_string);
  TEST_REPORT();
}



