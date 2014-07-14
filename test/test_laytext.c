/**
* This is a test for laytext in $package$
*/
#include <string.h>
#include "si_test.h"
#include "laytext.h"


static int callback(char * str, int bytes, void * extra, float * w, float * h) {
  (*w) = 1.0 * bytes; 
  (*h) = 1.0 * bytes;
  return bytes;
}


TEST_FUNC(laytext) {
  int index;
  long start;
  Dynar * result;
  long value = 0;
  int length;
  char * text = "Milennia have passed since mankind first traveled to the moon. " 
  "Civilizations rose as never before, yet to fall again. "
  "When all hope seemed lost, the 21 trees sprouted from the earth. "
  "They brought mysterious powers that protected and strengthened mankind. "
  "Hello µ world, this is me, 無 life should be\nfun for everyone!";
  result = laytext_layout(text, 28.0, callback, NULL);
  for(index = 0; index < dynar_size(result); index++) {    
    dynar_get_long(result, index, &value);
    printf("Layout: %d, %ld/%zd\n", index, value, strlen(text));
  }  
  
  length = strlen(text);
  start = 0;
  for(index = 0; index < dynar_size(result); index++) {    
    dynar_get_long(result, index, &value);
    if (value > 0) { 
      printf("|%.*s|%d, %ld, %ld|\n", (int)(value - start), text+start, (int)(value - start), value, start);
    } 
    start = value + 1;
  }
  printf("|%s|EOS|\n", text+start);
  
  
  dynar_free(result);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(laytext);
  TEST_REPORT();
}



