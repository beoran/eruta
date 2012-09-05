/**
* This is a test for obj in $package$
*/
#include "si_test.h"
#include "obj.h"


TEST_FUNC(obj) {
  void * object, * aid;
  object = obj_alloc(123, NULL);
  TEST_NOTNULL(object);
  TEST_NOTNULL(obj_ref(object));
  TEST_NOTNULL(obj_unref(object));
  TEST_NULL(obj_unref(object));
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(obj);
  TEST_REPORT();
}



