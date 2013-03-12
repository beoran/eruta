/**
* This is a test for castor in $package$
*/
#include "si_test.h"
#include "castor.h"

char * resources[] = {
  "zero",
  "one",
  "two",
  "three",
  "four",
  "five",
  "six"
};

void * load_fake_resource(const char * name, int type, void * extra) {
  int index = atoi(name);
  if(index < 0) return NULL;
  if(index >= sizeof(resources)) return NULL;
  return resources[index];
}

void * unload_fake_resource(void * data, void * extra) {
  return NULL;
} 

#define FAKE_RES_TYPE 30

CastorType fake_type = {
  load_fake_resource, unload_fake_resource, FAKE_RES_TYPE  
};



TEST_FUNC(castor) {
  Castor * castor = castor_new();
  TEST_NOTNULL(castor);
  castor_addtype(castor, fake_type);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(castor);
  TEST_REPORT();
}


