#include "testing.h"
#include "raku.h"
#include <stdio.h>

TEST_FUNC(raku_mem) {
  void * mem;
  mem = ra_mem_alloc(123);
  ra_mem_free(mem);  
  TEST_DONE();
}


TEST_FUNC(raku_string) {
  RaString s1, s2, s3, s4;
  fprintf(stderr, "sizeof RaVar : %d\n", sizeof(RaVar)); 
  s1 = ra_string_fromc("Hello");
  ra_string_puts(s1);
  s2 = ra_string_fromc(" World!");
  ra_string_puts(s2);
  s3 = ra_string_new(NULL, 0);
  s3 = ra_string_concat(s3, s1, s2);  
  ra_string_puts(s3); 
  s4 = ra_string_new(NULL, 0);
  s4 = ra_string_slice(s4, s3, 0, 5);
  ra_string_puts(s4);
  
  RA_OBJECT_TOSS(s1);
  RA_OBJECT_TOSS(s2);
  RA_OBJECT_TOSS(s3);
  RA_OBJECT_TOSS(s4);
  TEST_DONE();
}

TEST_FUNC(raku_array) {
  RaVar   v1 = RA_INT(69) , v2 = RA_NUM(2.0), v3, v4;
  RaArray a1;  
  TEST_INTEQ(69, v1.val.i);
  a1 = ra_array_new();
  ra_array_push(a1, v1);
  ra_array_push(a1, v2);
  TEST_INTEQ(69, ra_array_get(a1, 0).val.i);
  // TEST_DBLEQ(2.0, ra_array_get(a1, 1).val.n);
  RA_OBJECT_TOSS(a1);
  
  TEST_DONE();
}


int main(int argc, char * argv[]) {
  int result;
  
  TEST_INIT(); 
  TEST_RUN(raku_mem);
  TEST_RUN(raku_string);
  TEST_RUN(raku_array);
  TEST_REPORT();
}



