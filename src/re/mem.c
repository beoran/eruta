#include <stdlib.h>
#include <string.h>
#include "re/mem.h"


void * re_malloc(size_t size) {
  if (size < 1) return NULL;
  return calloc(size, 1);
}

void * re_free(void * ptr) {
 free(ptr);
 return NULL;
}

void * re_memcpy(void * dst, void * src, size_t size) {
  size_t index = 0;
  if(!dst) return NULL;
  if(!src) return NULL;
  for (index = 0 ; index < size ; index++) {
    ((char *)dst)[index] = ((char *)src)[index];
  }
  return dst;
}


void * re_realloc(void * ptr, size_t size) {
  return realloc(ptr, size);
}


struct ReMem_ {
  void * data;
  size_t size;
};


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




