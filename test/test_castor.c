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

void * load_fake_resource(const char * name, int type, int flags, void * extra) {
  int index = atoi(name);
  if(index < 0) return NULL;
  if((unsigned int)index >= sizeof(resources)) return NULL;
  return strdup(resources[index]);
}

void * unload_fake_resource(void * data, void * extra) {
  free(data);
  return NULL;
} 

#define FAKE_RES_TYPE 30

CastorType fake_type = {
  FAKE_RES_TYPE, load_fake_resource, unload_fake_resource   
};



TEST_FUNC(castor) {
  void * aid;
  Castor * castor = castor_new();
  TEST_NOTNULL(castor);
  TEST_NOTNULL(castor_addtype(castor, fake_type.index, &fake_type));
  aid = castor_load(castor, "2", FAKE_RES_TYPE);
  TEST_NOTNULL(aid);
  TEST_STREQ((const char *)aid, "two"); 
  aid = castor_load(castor, "4", FAKE_RES_TYPE);
  TEST_NOTNULL(aid);
  TEST_STREQ((const char *)aid, "four"); 
  aid = castor_load(castor, "2", FAKE_RES_TYPE);
  TEST_NOTNULL(aid);
  TEST_STREQ((const char *)aid, "two"); 
  aid = castor_load(castor, "4", FAKE_RES_TYPE);
  TEST_NOTNULL(aid);
  TEST_STREQ((const char *)aid, "four"); 
  
  TEST_NULL(castor_free(castor));
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(castor);
  TEST_REPORT();
}


