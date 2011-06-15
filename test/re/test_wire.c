#include <stdio.h>
#include "re/test.h"
#include "re/wire.h"

TEST_FUNC(rewire_new) {
  ReWire * wire = rewire_new("Hello");
  TEST_NOTNULL(wire);
  TEST_NOTNULL(rewire_cstr(wire));
  TEST_FALSE(rewire_const_p(wire));
  TEST_INTEQ(rewire_size(wire), 5);
  TEST_INTEQ(rewire_room(wire), 11);
  TEST_ASSERT(rewire_puts(wire) > 0);
  rewire_free(wire);
  TEST_DONE();
}

TEST_FUNC(rewire_const) {
  ReWire * wire = rewire_const("Hello");
  TEST_NOTNULL(wire);
  TEST_TRUE(rewire_const_p(wire));
  TEST_NOTNULL(rewire_cstr(wire));
  TEST_INTEQ(rewire_size(wire), 5);
  TEST_INTEQ(rewire_room(wire), 0);
  TEST_ASSERT(rewire_puts(wire) > 0);
  rewire_free(wire);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(rewire_new);
  TEST_RUN(rewire_const);
  TEST_REPORT();
}



