#include "si_test.h"
#include "program.h"



TEST_FUNC(eruta_program) {
  /*Program * prog = program_new(640, 480, FALSE);
  TEST_NOTNULL(prog);
  program_free(prog);
*/ 
  TEST_DONE();
}

int main(void) {
  TEST_INIT();
  TEST_RUN(eruta_program);
  TEST_REPORT();
}
