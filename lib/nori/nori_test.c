#include "nori.h"

#include "testing.h"
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/times.h>

 


#define ARRAY_SIZE 123

typedef void * vptr;
typedef vptr (funcptr)();
 
int do_nothing(int i) {  
  return i;
} 
 
int sum(int i1, int i2) {
  return i1 + i2;
} 

#define TRIES 1000000000

TEST_FUNC(nori) {
  clock_t   start, stop, delta; 
  double d1 = 1.0, d2 = 1.0;
  int i, j;
  start = clock();  
  for(i = 0; i < TRIES; i++) {
    j = do_nothing(i);
  }
  stop  = clock();
  delta = stop - start;
  d1    = ((double) delta) / ((double)CLOCKS_PER_SEC);
  fprintf(stderr, "Do nothing %d times: %d (%lf/s) %d\n", TRIES, delta, d1, j);  
  TEST_DONE();
}


int main(int argc, char * argv[]) {
  int result;
  
  TEST_INIT(); 
  TEST_RUN(nori);
  TEST_REPORT();
}









