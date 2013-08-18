/**
* This is a test for bnobject
*/
#include "si_test.h"
#include "bn/BNObject.h"

@interface TestClass : BNObject 
@end

@implementation TestClass
@end


TEST_FUNC(bnobject) {
  id obj;
  obj = [[TestClass alloc] init];
  TEST_NOTNULL(obj);  
  TEST_NULL([obj free]);  
  obj = [[TestClass alloc] init];
  TEST_INTEQ([obj references], 1);
  TEST_PTREQ([obj retain], obj);
  TEST_INTEQ([obj references], 2);
  TEST_PTREQ([obj release], obj);
  TEST_INTEQ([obj references], 1);
  TEST_NULL([obj release]);
  
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(bnobject);
  TEST_REPORT();
}


