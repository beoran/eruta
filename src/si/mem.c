#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "si_mem.h"


void * si_malloc(size_t size) {
  if (size < 1) return NULL;
  return calloc(size, 1);
}

void * si_free(void * ptr) {
 free(ptr);
 return NULL;
}


void * si_smemcpy(void * dst, size_t dsts, size_t dsti, 
                  void * src, size_t srcs, size_t srci, 
                  size_t amount) {
  size_t index = 0;
  if(!dst) return NULL;
  if(!src) return NULL;
  while((dsti < dsts) && (srci < srcs) && (index < amount)) {
    uint8_t c               = ((uint8_t *)src)[srci];
    ((uint8_t *)dst)[dsti]  = c;
    dsti++; srci++; index++; 
  } 
  return dst;
}

void * si_memcpy(void * dst, void * src, size_t size) {
  return si_smemcpy(dst, size, 0 , src , size, 0, size);  
}


void * si_realloc(void * ptr, size_t size) {
  return realloc(ptr, size);
}

/** Allocate and copy memory. */
void * si_copyalloc(size_t size, void * src,  size_t tocopy) {
  void * res = si_malloc(size);
  if(!res) return NULL;
  return si_memcpy(res,src,tocopy); 
}


struct ReMem_ {
  void * data;
  size_t size;
};

/*
ReMem * remem_init(ReMem* self, size_t size) {
  if(!self) return NULL;  
  self->size = size;
  self->data = re_malloc(size);  
  if(self->data) return self;
  return NULL;
}

ReMem * remem_new(size_t size) {
  ReMem * res = re_malloc(sizeof(ReMem));
  void * ptr  = NULL; 
  if(!res) return NULL;
  ptr         = re_malloc(size); // allocate pointer. 
  if (ptr) return remem_init(res, ptr, size);
  
}

ReMem * remem_resize(ReMem * self, size_t size);
ReMem * remem_dup(ReMem * self);
ReMem * remem_copy(ReMem * self, ReMem * mem, size_t size);
ReMem * remem_xcopy(ReMem * self, ReMem * mem, size_t iself, size_t imem, size_t size);
ReMem * remem_free(ReMem * self);

size_t remem_size(ReMem * self);
void * remem_ptr(ReMem * self);
*/



