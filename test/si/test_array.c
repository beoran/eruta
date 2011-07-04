/**
* This is a test for array in si
*/
#include "si_test.h"
#include "si_array.h"

typedef struct TestArray1_ {
  int     a;
  char    b;
} TestArray1;

TEST_FUNC(si_array) {
  TestArray1 ts1 = { 54321, 123 };
  TestArray1 * tsp; 
  SiArray * parr = siarray_newptr(64); // pointer array
  SiArray * darr = siarray_new(64, sizeof(TestArray1)); // data array
  TEST_INTEQ(0, siarray_size(darr));
  TEST_INTEQ(0, siarray_size(parr));
  TEST_INTEQ(64, siarray_room(darr));
  TEST_INTEQ(64, siarray_room(parr));
  TEST_NULL(siarray_size_(parr, 77));
  TEST_NULL(siarray_size_(darr, 88));
  TEST_NOTNULL(siarray_size_(parr, 33));
  TEST_NOTNULL(siarray_size_(darr, 44));
  TEST_INTEQ(33, siarray_size(parr));
  TEST_INTEQ(44, siarray_size(darr));
  TEST_NOTNULL(siarray_size_(parr, 0));
  TEST_NOTNULL(siarray_size_(darr, 0));
  
  TEST_NULL(siarray_putptr(parr, 123, NULL));
  TEST_NULL(siarray_getptr(parr, 123));
  TEST_NULL(siarray_putdata(darr, 123, NULL));
  TEST_NULL(siarray_getdata(darr, 123));
  TEST_NOTNULL(siarray_putptr(parr, 12, &ts1));
  tsp = siarray_getptr(parr, 12);
  TEST_NOTNULL(tsp);
  TEST_MEMEQ(&ts1, sizeof(TestArray1), tsp);
  TEST_NOTNULL(siarray_putdata(darr, 7, &ts1));
  tsp = siarray_getdata(darr, 7);
  TEST_NOTNULL(tsp);
  TEST_MEMEQ(&ts1, sizeof(TestArray1), tsp);
  TEST_NOTNULL(siarray_putdata(darr, 12, &ts1));
  TEST_NOTNULL(siarray_getdata(darr, 12));

  TEST_NOTNULL(siarray_grow(parr, 32));
  TEST_NOTNULL(siarray_grow(parr, 128));
  TEST_MEMEQ(&ts1, sizeof(TestArray1), siarray_getptr(parr, 12));
  TEST_NOTNULL(siarray_putptr(parr, 123, &ts1));
  TEST_MEMEQ(&ts1, sizeof(TestArray1), siarray_getptr(parr, 123));
  TEST_PTREQ(&ts1, siarray_getptr(parr, 123));
  TEST_NOTNULL(siarray_size_(parr, 77));


  siarray_free(parr);
  siarray_free(darr);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(si_array);
  TEST_REPORT();
}



