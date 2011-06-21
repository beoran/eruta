#include <stdio.h>
#include <stddef.h>
#include "si_test.h"
#include "si_wire.h"

TEST_FUNC(siwire_new) {
  SiWire * wire = siwire_new("Hello");
  TEST_NOTNULL(wire);
  TEST_NOTNULL(siwire_cstr(wire));
  TEST_FALSE(siwire_const_p(wire));
  TEST_INTEQ(siwire_size(wire), 5);
  TEST_INTEQ(siwire_room(wire), 11);
  TEST_ASSERT(siwire_puts(wire) > 0);
  siwire_free(wire);
  TEST_DONE();
}

TEST_FUNC(siwire_const) {
  SiWire * wire = siwire_const("Hello");
  TEST_NOTNULL(wire);
  TEST_TRUE(siwire_const_p(wire));
  TEST_NOTNULL(siwire_cstr(wire));
  TEST_INTEQ(siwire_size(wire), 5);
  TEST_INTEQ(siwire_room(wire), 0);
  TEST_ASSERT(siwire_puts(wire) > 0);
  siwire_free(wire);
  TEST_DONE();
}

TEST_FUNC(siwire_dup) {  
  SiWire * wire = siwire_const("Hello");
  SiWire * wired = siwire_dup(wire);
  TEST_NOTNULL(wire);
  TEST_TRUE(siwire_const_p(wire));
  TEST_FALSE(siwire_const_p(wired));
  TEST_NOTNULL(siwire_cstr(wire));
  TEST_NOTNULL(siwire_cstr(wired));  
  TEST_INTEQ(siwire_size(wire), 5);
  TEST_INTEQ(siwire_size(wired), 5);
  siwire_free(wire);
  siwire_free(wired);
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
  puts(SI_OS);
  TEST_RUN(siwire_new);
  TEST_RUN(siwire_const);
  TEST_RUN(siwire_dup);
  TEST_REPORT();
}



