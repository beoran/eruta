/**
* This is a test for bxml in $package$
*/
#include "si_test.h"
#include "bxml.h"


TEST_FUNC(bxml) {
  Bxml * xml;
  xml = bxml_new("?xml", BXML_TAG);
  TEST_NOTNULL(xml);
  TEST_NOTNULL(bxml_new_attribute(xml, "hello" , "world"));
  TEST_NOTNULL(bxml_new_attribute(xml, "hello2", "world2"));
  TEST_NOTNULL(bxml_new_attribute(xml, "hello3", "world3"));
  
  
  TEST_STREQ("world2", bxml_get_attribute(xml, "hello2"));
  TEST_STREQ("world", bxml_get_attribute(xml, "hello"));
  TEST_STREQ("world3", bxml_get_attribute(xml, "hello3"));
  TEST_NULL(bxml_get_attribute(xml, "hello22"));
  
  bxml_free(xml);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(bxml);
  TEST_REPORT();
}



