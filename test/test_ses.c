/**
* This is a test for ses in $package$
*/
#include "si_test.h"
#include "ses.h"


TEST_FUNC(ses) {
  char * buf;
  size_t size;
  size_t space;
  
  TEST_NOTNULL(ses_new(&buf, &size, &space, "he"));
  TEST_INTEQ(2, size);
  TEST_ASSERT(space > size); 
  TEST_NOTNULL(ses_append_cstr(&buf, &size, &space, "llo"));
  TEST_INTEQ(5, size);
  TEST_ASSERT(space > size); 
  TEST_NOTNULL(ses_append_buf(&buf, &size, &space, " world", 5));
  TEST_INTEQ(10, size);
  TEST_ASSERT(space > size);
  TEST_NOTNULL(ses_append_char(&buf, &size, &space, 'd'));
  TEST_STREQ("hello world", buf);
  TEST_INTEQ(11, size);
  TEST_ASSERT(space > size);
  TEST_NOTNULL(ses_make_empty(&buf, &size, &space));
  TEST_STREQ("", buf);
  TEST_INTEQ(0, size);
  TEST_ASSERT(space > size);
  
  TEST_NOTNULL(ses_read_filename(&buf, &size, &space, "test/test_ses.txt"));
  TEST_STREQ("input\nfrom file\n", buf);
  
  ses_free(&buf, &size, &space);
  TEST_INTEQ(0, size);
  TEST_INTEQ(0, space);
  
  
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(ses);
  TEST_REPORT();
}



