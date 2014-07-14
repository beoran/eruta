/**
* This is a test for dynar in $package$
*/
#include "si_test.h"
#include "dynar.h"
#include <string.h>


int dynar_string_compare(const void * one, const void * two) {
  if(!one) {
    if(!two) return 0;
    return 1;
  }
  if(!two) return -1;
  if(!*(char*)one) {
    if(!*(char *)two) return 0;
    return 1;
  }
  if(!*(char *)two) return -1;
  
  printf("Compare: %s, %s\n",*((const char**) one), *((const char**) two));  
  return strcmp( *((const char**) one), *((const char**) two));  
}




TEST_FUNC(dynar) {
  int index;
  const char * t = "wine";
  Dynar * arr;
  arr = dynar_newptr(4);
  TEST_NOTNULL(dynar_putptr(arr, 0, "zebra"));
  TEST_NOTNULL(dynar_putptr(arr, 1, "apple"));
  TEST_NOTNULL(dynar_putptr(arr, 2, "bread"));
  TEST_NOTNULL(dynar_putptr(arr, 3, "wine"));
  dynar_grow(arr, 9);
  TEST_NOTNULL(dynar_putptr(arr, 4, "thyme"));
  for(index = 5; index < dynar_size(arr); index ++) {
    dynar_putptr(arr, index, NULL);
  }
  
  for(index = 0; index < dynar_size(arr); index ++) {
    char * str = dynar_getptr(arr, index);
    if(str) { 
      printf("Array index %d : %s\n", index, str);
    } else {
      printf("Array index %d : NULL\n", index);     
    }
  }
  
  
  TEST_NOTNULL(dynar_qsort(arr, dynar_string_compare));
  printf("After qsort:\n");
  for(index = 0; index < dynar_size(arr); index ++) {
    char * str = dynar_getptr(arr, index);
    printf("Array index %d : %s\n", index, str);
  }

  TEST_NOTNULL(dynar_bsearch(arr, &t, dynar_string_compare));  
  TEST_DONE();
}

TEST_FUNC(dynar_long) {
  long value = 0;
  Dynar * arr = dynar_new_long();
  dynar_grow(arr, 10);  
  TEST_NOTNULL(dynar_put_long(arr, 3, 12345678));
  TEST_NOTNULL(dynar_get_long(arr, 3, &value));
  TEST_LONGEQ(value, 12345678);
  TEST_NOTNULL(dynar_append_long(arr, 9876));
  TEST_NOTNULL(dynar_get_long(arr, 10, &value));
  TEST_LONGEQ(value, 9876);
  TEST_NOTNULL(dynar_append_long(arr, 9876));
  TEST_NOTNULL(dynar_get_long(arr, 11, &value));
  TEST_LONGEQ(value, 9876);
  TEST_NOTNULL(dynar_append_long(arr, 9876));
  TEST_NOTNULL(dynar_get_long(arr, 12, &value));
  TEST_LONGEQ(value, 9876);
  TEST_NOTNULL(dynar_append_long(arr, 9876));
  TEST_NOTNULL(dynar_get_long(arr, 13, &value));
  TEST_LONGEQ(value, 9876);
  TEST_NOTNULL(dynar_append_long(arr, 9876));
  TEST_NOTNULL(dynar_get_long(arr, 14, &value));
  TEST_LONGEQ(value, 9876);
  
  TEST_NOTNULL(dynar_append_long(arr, 9876));
  TEST_NOTNULL(dynar_get_long(arr, 15, &value));
  TEST_LONGEQ(value, 9876);
  TEST_NOTNULL(dynar_append_long(arr, 9876));
  TEST_NOTNULL(dynar_get_long(arr, 16, &value));
  TEST_LONGEQ(value, 9876);
  TEST_NOTNULL(dynar_append_long(arr, 9876));
  TEST_NOTNULL(dynar_get_long(arr, 17, &value));
  TEST_LONGEQ(value, 9876);
  
  
  TEST_DONE();
}

int main(void) {
  TEST_INIT();
  TEST_RUN(dynar);
  TEST_RUN(dynar_long);
  TEST_REPORT();
}



