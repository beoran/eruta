/* Memory handling, malloc wrapers, etc */

#include <stdlib.h>
#include <string.h>
#include "mem.h"

/** Wrapper for calloc/malloc */
void * mem_calloc(size_t amount, size_t size) {
  return calloc(amount, size);
}

/** Wrapper for calloc/malloc */
void * mem_alloc(size_t size) {
  return mem_calloc(1, size);
}

/** Wrapper for free */
void * mem_free(void * ptr) {
  free(ptr);
  return NULL;
}

/** Wrapper for realloc */
void * mem_realloc(void *ptr, size_t newsize) {
  return realloc(ptr, newsize);
}

/** Resizes memory, taking care not to lose ptr* if the reallocation
fails. */
void * mem_resize(void ** ptr, size_t newsize) {
  if(*ptr) { 
    void * res = mem_realloc((*ptr), newsize);
    if(res) {
      (*ptr) = res;
      return res;
    }
  }  
  return NULL;
}

/** Wrapper for memmove, for consistency */
void * mem_move(void * dest, void * src, size_t size) {
  return memmove(dest , src, size);
}




