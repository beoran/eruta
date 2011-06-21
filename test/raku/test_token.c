#include "si_test.h"
#include "si_wire.h"
#include "raku_token.h"


TEST_FUNC(token) {
  SiWire * wire   = siwire_new("Hello");
  RakuToken * tok = rakutoken_new(RakuTokenString, wire, 10, 3);  
  TEST_NOTNULL(tok);
  TEST_INTEQ(rakutoken_line(tok), 10);
  TEST_INTEQ(rakutoken_col(tok), 3);
  TEST_ASSERT(siwire_equalcstr_p(rakutoken_value(tok), "Hello"));
  rakutoken_free(tok);
  siwire_free(wire);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(token);
  TEST_REPORT();
}
