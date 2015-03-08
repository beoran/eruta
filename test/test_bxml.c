/**
* This is a test for bxml in $package$
*/
#include "si_test.h"
#include "bxml.h"


TEST_FUNC(bxml) {
  Bxml * xml, * c1, * c2, * c3, * t1, * t2, * t3;
  xml = bxml_new(BXML_ROOT);
  TEST_NOTNULL(xml);
  TEST_NOTNULL(bxml_new_attribute(xml, "hello" , "world"));
  TEST_NOTNULL(bxml_new_attribute(xml, "hello2", "world2"));
  TEST_NOTNULL(bxml_new_attribute(xml, "hello3", "world3"));
  
  
  TEST_STREQ("world2", bxml_get_attribute(xml, "hello2"));
  TEST_STREQ("world", bxml_get_attribute(xml, "hello"));
  TEST_STREQ("world3", bxml_get_attribute(xml, "hello3"));
  TEST_NULL(bxml_get_attribute(xml, "hello22"));
  
  c1 = bxml_new_child(xml, BXML_TAG , "map");
  c2 = bxml_new_child(xml, BXML_TEXT, "this is my text");
  c3 = bxml_new_child(c1, BXML_TAG, "level");
  
  t1 = bxml_get_sibling_at(xml, 0);
  TEST_NULL(bxml_get_sibling_at(xml, 1));
  
  TEST_NOTNULL(t1);
  TEST_PTREQ(xml, t1);
  
  t1 = bxml_get_child_at(xml, 0);
  t2 = bxml_get_child_at(xml, 1);
    
  TEST_PTREQ(c1, t1);
  TEST_PTREQ(c2, t2);
  
  
  bxml_free(xml);
  TEST_DONE();
}


TEST_FUNC(bxml_parse) {
  Bxml * xml;  
  xml = bxml_parse_filename("data/map/map_0001.tmx");
  TEST_NOTNULL(xml);
  bxml_free(xml);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(bxml);
  TEST_RUN(bxml_parse);
  TEST_REPORT();
}



