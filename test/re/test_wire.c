#include <stdio.h>
#include <stddef.h>
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

struct Mixin_ {
  size_t refs;
};


typedef struct Mixin_ Mixin;

#define mixin(NAME) NAME NAME 

struct Buf_ {
  char * data;
  size_t size;
  mixin(Mixin);
  size_t space;
};

struct Cnt_ {
  int cnt;  
  mixin(Mixin);
};


typedef struct Buf_ Buf;

#define container_of(DATA, TYPE, FIELD) \
  ((TYPE*)(((char *)(&(DATA))) - offsetof(TYPE, FIELD)))

#define mixin_of(DATA, MIXTYPE) \
  ((MIXTYPE *)&((DATA)->MIXTYPE))

int main(void) {
  Buf buf;
  Buf * cont;
  Mixin * mix;
  TEST_INIT();
  printf("Offsetof mix1: %u\n", offsetof(Buf, Mixin));
  cont = container_of((buf.Mixin), Buf, Mixin);
  mix  = mixin_of(&buf, Mixin); 
  //cont = (Buf*)(((char *)&buf.mix1) - offsetof(Buf, mix1));
  printf("&buf, cont, mix: %p %p %p %d\n", &buf, cont, mix, cont == &buf); 
  TEST_RUN(rewire_new);
  TEST_RUN(rewire_const);
  TEST_REPORT();
}



