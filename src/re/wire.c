#include <stdlib.h>
#include "re/wire.h"


struct ReWire_ {
  char * data;
  size_t size;
  size_t room;
};

/*
static size_t re_strlen(char * str) {
  int res;   
  if(!str) return NULL;
  for(res = 0; (*str) != '\0' ; str++ ) { res++; }
  return res;  
};

static char * re_strdup(char * str) {
  char * res;
  int ind, len;
  if(!str) return NULL;
  len = re_strlen(str);
  res = re_malloc(len);
  for(ind = 0; ind < len; ind++) { res[ind] = str[ind]; } 
  return res;
}



ReWire * rewire_init(ReWire * self, char * str, int size, int room) {
  if (room == 0) // constant string {
    self->data = str;
    self->size = re_strlen(str);
    self->room = room;  
  } else {
    self->data = re_strdup(str);
    self->size = re_strlen(str);
    self->room = room;
  }
};
*/








