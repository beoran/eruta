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


/* OOP style type info */
struct TypeInfo_ {
  struct TypeInfo_   * up;
  MemDestructor      * done;
};

/* Reference counted memory with OOP- style functionality. */
struct MemHeader_ {
  struct TypeInfo_  * type;
  int                 refcount;
};


void * obj_alloc(size_t size, struct TypeInfo_ * type) {
  char * ptr                = mem_alloc(size + sizeof(struct MemHeader_));
  struct MemHeader_  * aid  = (struct MemHeader_ *) ptr;
  if(!aid) return NULL;
  aid->type                 = type;
  aid->refcount             = 1;
  return aid + sizeof(struct MemHeader_);
}

struct MemHeader_ * obj_memheader(void * ptr) {
  char * aid;
  if(!ptr) return NULL;
  aid = (char *) ptr;
  return (struct MemHeader_ *) (aid - sizeof(struct MemHeader_));
}

void * obj_ref(void * ptr) {
  struct MemHeader_ * header;
  header = obj_memheader(ptr);
  if(!header) return NULL;
  header->refcount++;
  return ptr;  
}

void * obj_done(void * ptr) {
  struct MemHeader_ * header;
  struct TypeInfo_  * info;
  header = obj_memheader(ptr);
  if(!header) return NULL;
  if(!header->type) return NULL;
  // look for a destructor in the linked type info tables
  for (info = header->type; info->up; info = info->up) {
    if(info->done) {
      return info->done(ptr);
    }
  }
  // if we get here do nothing.
  return ptr;
}

void * obj_unref(void * ptr) {
  struct MemHeader_ * header;
  header = obj_memheader(ptr);
  if(!header) return NULL;
  header->refcount--;
  if(header->refcount < 1) {
    obj_done(ptr);
    mem_free(header); // if we free the header, the rest of the 
    // object which follows should be freed as well.
    return NULL;
  }
  return ptr;
}


