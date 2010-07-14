#include "rtcutl.h"
#include "spec.h"


#define ASIZE 10

describe(Array) { 
  Array a; 
  int buf[ASIZE];
  
  it(can be initialized) {
    int i;
    i = 1;
    should_not_be_null(array_init(&a, buf, ASIZE, sizeof(int)));
  }

} done




