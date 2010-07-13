#include "rtcutl.h"
#include "testing.h"


#define BUFFER_SIZE 123

Test * test_queue(Test * test) {
  int val;
  int buffer[BUFFER_SIZE];
  Queue queue;
  queue_init(&queue, &buffer, BUFFER_SIZE, sizeof(int));
  test_null(test, queue_shift(&queue, &val), 
            "Empty queue should not be shifted.");
  val = 123; 
  test_notnull(test, queue_push(&queue, &val), "Push to queue failed.");
  val = 456; queue_push(&queue, &val);
  val = 789; queue_push(&queue, &val);
  queue_shift(&queue, &val);
  test_assert(test, val == 123, "Shifted wrong value from queue.");
  queue_shift(&queue, &val);
  test_assert(test, val == 456, "Shifted wrong value from queue.");
  queue_shift(&queue, &val);
  test_assert(test, val == 789, "Shifted wrong value from queue.");
  test_null(test, queue_shift(&queue, &val), 
            "Empty queue should not be shifted.");
  return test;
}

#define STICK_CAP 16
Test * test_stick(Test * test) {
    Stick sstick1, sstick2;
    char buffer[STICK_CAP];
    char buffer2[STICK_CAP];
    Stick *stick  = &sstick1;
    Stick *stick2 = &sstick2;
    test_notnull(test, stick_init(stick, buffer, STICK_CAP),"stick_init");   
    test_assert(test, stick_cap(stick)  == STICK_CAP, "Wrong capacity!");
/*
    test_assert(test, stick_len(stick)  == 7, "Wrong length!");
    test_assert(test, stick_size(stick) == 6, "Wrong size!");
*/
    stick_strcpy(stick, "Hello!");
    test_streq(test, stick_cstr(stick), "Hello!", "stick_strcpy");
    stick_strncpy(stick, "Hello!", 2);
    test_streq(test, stick_cstr(stick), "He", "stick_strncpy");
//Stick * stick_copy(Stick* dst, Stick * src); 
//Stick * stick_cat(Stick * dst, Stick * src); 
//Stick * stick_trunc(Stick * stick, size_t len); 
// Stick * stick_xcopy(Stick * dst, Stick * src, 
//        size_t len, size_t sstart, size_t dstart); 

// Gets the first len characters from the left side of 
// the stick src and copies them to dst, which will be 
// truncated to length 1 first.  
// Stick * stick_left(Stick * dst, Stick * src, size_t len);
}


#define ARRAY_CAP 16

Test * test_array(Test * t) {
  Array sarray1, sarray2; 
  int get;
  int src1[] = { 1, 2, 3, 4 };
  int src2[] = { 5, 6, 7, 8 };
  int res[]  = { 1, 2, 3, 4, 5, 6, 7, 8 };
  int buffer1[ARRAY_CAP];
  int buffer2[ARRAY_CAP];
  int aid = 0;
  // int * buffer1;
  // int * buffer2;
 
  Array *array1 = &sarray1;
  Array *array2 = &sarray2;
  test_notnull(t, array_init(array1, buffer1, ARRAY_CAP, sizeof(int)), 
    "array_init");
  test_notnull(t, array_init(array2, buffer2, ARRAY_CAP, sizeof(int)), 
    "array_init");
  array_memcpy(array1, src1, 4);
  array_memcpy(array2, src2, 4); 
  test_inteq(t, array_len(array1), 4, "array_len");
  test_inteq(t, array_cap(array1), 16, "array_cap");
  
  test_memeq(t, (int *)array1->ptr, src1, 4, "array_memcpy");
  
  array_cat(array1, array2);
  test_notnull(t, array_cat(array1, array2), "array_cat");    
  test_memeq(t, (int *)array1->ptr, res, 8 , "array_cat");
  for (aid = 0; aid < 8; aid++) { 
    array_get(array1, aid, &get);  
    test_inteq(t, get, aid + 1, "array_cat");
  }
    
  array_get(array1, 3, &get); 
  test_inteq(t, get, 4, "array_get");
  get = 5; 
  array_set(array1, 4, &get);
  array_get(array1, 4, &get);
  test_inteq(t, get, 5, "array_set");  
  test_null(t, array_set(array1, 16, &get), "array_set respects limits");
  test_null(t, array_get(array1, 16, &get), "array_get respects limits");
  test_null(t, array_set(array1, -1, &get), "array_set respects neg limits");
  test_null(t, array_get(array1, -1, &get), "array_get respects neg limits");
  test_notnull(t, array_set(array1, 15, &get), "array_set respects  limits correctly");
  test_notnull(t, array_get(array1, 15, &get), "array_get respects limits correctly");
  
  
  test_notnull(t, array_trunc(array1, 0), "array_trunc");
  test_inteq(t, array_len(array1), 0, "array_trunc");
  get = 5; 
  for (aid = 0; aid < 16; aid++) {
    array_set(array1, aid, &get);
    test_inteq(t, array_len(array1), aid + 1, "array_set expands len");
  }  
  test_notnull(t, array_trunc(array1, 0), "array_trunc");
  test_inteq(t, array_len(array1), 0, "array_trunc resets len");
  get = 99; 
  test_notnull(t, array_fill(array1, &get), "array_fill");
  test_inteq(t, array_len(array1), 16, "array_fill expands len");
  array_get(array1, 15, &get);
  test_inteq(t, get, 99, "array_fill works");
  
  test_notnull(t, array_ptr(array1), "array_ptr");
     
  return t;
}

#define BYBUF_CAP 8
TEST_FUNC(bybuf) {
  uint8_t buffer1[BYBUF_CAP];
  uint8_t buffer2[BYBUF_CAP];
  Bybuf  sbybuf1,   sbybuf2; 
  Bybuf * bybuf1, * bybuf2;
  bybuf1 = &sbybuf1;
  bybuf2 = &sbybuf2;
  // Initialization
  TEST_NOTNULL(bybuf_init(bybuf1, buffer1, BYBUF_CAP));
  // Filling
  TEST_NOTNULL(bybuf_fill(bybuf1, 0));
  // Setting
  TEST_NOTNULL(bybuf_setb(bybuf1, 1, 99));
  // Getting
  TEST_INTEQ(99, bybuf_getb(bybuf1, 1));
  // Getting out range should return -1.
  TEST_INTEQ(-1, bybuf_getb(bybuf1, 99));
  
  TEST_DONE();
}


int main(void) {
  TEST_INIT(); 
  //Test test;
  //test_init(&test);
  TEST_RUN(queue);
  TEST_RUN(array);
  TEST_RUN(stick);
  TEST_RUN(bybuf);
  /*
  test_queue(&test);
  test_array(&test);
  test_stick(&test);
  test_bybuf(&test);
  */
  TEST_REPORT();
}








