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


/* pointer handler. */
typedef uint32_t Handle;

struct Meminfo_;
typedef struct Meminfo_ Meminfo;
struct Memlist_;
typedef struct Memlist_ Memlist;


struct Meminfo_ {
  void      * ptr;
  int         refcount;
  uint16_t    offset;
  uint8_t     usage;
  uint8_t     type;
  uint16_t    nextfree;
};

struct Memlist_ {
  int        size;
  Meminfo  * info;
  uint16_t   nextfree;
};

#define MEMLIST_SIZE 1024

Memlist * memlist_init(Memlist * self) {
  int index;
  self->size = MEMLIST_SIZE;
  self->info = mem_calloc(self->size, sizeof(Meminfo));
  if(!self->info) { self->size = 0; return NULL; }
  self->nextfree = 0;  
  for(index = 0; index < self->size; index ++) {
    Meminfo * aid  = self->info + index;
    aid->ptr       = NULL;
    aid->refcount  = -1;
    aid->usage     = 0;
    aid->type      = 0;
    aid->nextfree  = index + 1;
  }
  return self;
}

Handle handle_make(uint16_t offset, uint8_t usage, uint8_t type) {
  return ((uint32_t)offset) + (((uint32_t)usage)<<16) + (((uint32_t)type)<<24);
}

Handle memlist_add(Memlist * self, void * ptr, uint8_t type) {
  Meminfo * aid;
  if(self->nextfree >= self->size) {
    Meminfo * big = NULL;
    if(!mem_resize((void **)&self->info, sizeof(Meminfo) * self->size * 2)) {
      return 0;
    }
    self->size = self->size * 2;
  }  
  aid = self->info + self->nextfree;
  aid->ptr       = ptr;
  aid->usage++;
  aid->type      = type;
  aid->refcount  = 1;
  aid->offset    = self->nextfree;
  self->nextfree = aid->nextfree;
  aid->nextfree  = -1; // next is used, so not known.
  return handle_make(aid->offset, aid->usage, aid->type);
}










