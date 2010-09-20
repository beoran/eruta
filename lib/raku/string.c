#include "raku.h"


struct RaStringStruct_;

typedef struct RaStringStruct_ RaStringStruct;
typedef RaStringStruct       * RaString;
typedef const RaStringStruct * RaConstString;

struct RaStringStruct_ {
  char    * data;
  RaSize    size;
};


static char * ra_strncpy(char * dst, char * src, RaSize size) {
  RaSize i;
  for( i = 0 ; i < size ; i++) {
     dst[i] = src[i];
  } 
  i++;
  dst[i] = '\0';
  return dst;
}

RaString ra_string_init(RaString rope, char * data, RaSize size) {
  int i, space;
  space 	  = size + 1;
  rope->data      = ra_mem_allot(space);
  if(!rope->data) return NULL;
  ra_strncpy(rope->data, data, size);
  rope->size  	  = size;
  return rope;
}

   

RaString ra_string_new(char * data, RaSize size) {
  RaString string = ra_mem_allot(sizeof(RaString));
  if(!string) return NULL;
  if(!ra_string_init(string, data, size)) {
    free(string); 
    return NULL;
  }
  return string;
}

RaString ra_string_fromc(char * data) {
  RaSize size = strlen(data);
  return ra_string_new(data, size);
}


RaString ra_string_done(RaString string) {
  if(string && string->data) {
    ra_mem_free(string->data);
    string->data = NULL;
  }
  return string;
}

RaString ra_string_free(RaString string) {
  ra_mem_free(ra_string_done(string));
}







