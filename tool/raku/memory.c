/**
* Memory.c contains mmory handlinf functions, as well as generic 
* functions for RaObject and for use in reference counting.
*/

#include "raku.h"
#include <stdlib.h>
#include <string.h>

/** Allot (allocate) memory block of the given size. May return null if the underlying system call fails. */
RA_FUNC(void *) ra_mem_alloc(RaSize size) {
  return calloc(size, 1);
}

/** Frees the memory block. meme may also be NULL in which case nothing 
happens. */
RA_FUNC(void) ra_mem_free(void * mem) {
  free(mem);
}

/** Reallocates a memory area to have the given size.  */
RA_FUNC(void *) ra_mem_resize(void * mem, RaSize size) {
  return realloc(mem, size);
}

/** Sets size bytes of the area poined to by dst to c. */
RA_FUNC(void *) ra_mem_set(void    * dst, int c, RaSize size) {
  return memset(dst, c, size);
}  

/** Zeroes size bytes of dst. */  
RA_FUNC(void *) ra_mem_zero(void   * dst, RaSize size) {
  return memset(dst, 0, size);
}

/** Moves memory from src to dst. the areas may overlap. */
RA_FUNC(void *) ra_mem_move(void   * dst, const void * src, RaSize size) {
  return memmove(dst, src, size);  
}


/** Some limits. */
#define RA_OBJECT_MAXREF  (65000)
#define RA_OBJECT_PERMREF (RA_OBJECT_MAXREF - 1)


/** Generic Raku object initialization function. */
RA_FUNC(RaObject) ra_object_init(RaObject obj, RaDestructor des) {
  if(!obj)       return NULL;
  obj->count_    = 1;
  obj->free_     = des;
  return obj;
}

/** Generic Raku object finalization function. */
RA_FUNC(void) ra_object_free(RaObject obj) {
  if(!obj || !obj->free_) return;
  obj->free_((RaPtr)obj);  // call destructor. 
  // Can't do obj->free_ = NULL, since obj has already been erased.   
}


/** Increase refcount, taking care not to exceed the max.  
Returns the object, or NULL if it was already null.
*/
RA_FUNC(RaObject) ra_object_use(RaObject obj) {
  if(!obj) return NULL;
  if(obj->count_ < RA_OBJECT_MAXREF) {
    obj->count_++;
  }
  return obj;
}
  
/** Decrease refcount, possibly calling the destructor if the refcount == 0.
Returns the object, or NULL if it was destroyed. 
**/
RA_FUNC(RaObject) ra_object_toss(RaObject obj) {
  if(!obj)          return NULL;
  if(!obj->count_)  return NULL;
  if(obj->count_ < RA_OBJECT_PERMREF) {
    obj->count_--;
  }  
  if(!obj->count_)  {
    ra_object_free(obj);
    return NULL;
  }
  return obj;
}

/** Increases the ref count on an object contained in a ra_var if 
it contains a pointer. */
RA_FUNC(RaVar) ra_var_use(RaVar var) {
  if (var.type > RA_TYPE_NUMBER) { 
    RA_OBJECT_USE(var.val.p);
  }  
  return var;
};


/** Decreases the ref count on an object contained in a ra_var if 
it contains a pointer. */
RA_FUNC(RaVar) ra_var_toss(RaVar var) {
  if (var.type > RA_TYPE_NUMBER) { 
    RA_OBJECT_TOSS(var.val.p);
  }
  return ra_fail_new();
};

/** Constructs a RaVar that signals failure. Used like in Icon. */
RA_FUNC(RaVar) ra_fail_new() {
  RaVar          result;
  result.type  = RA_TYPE_FAIL;
  result.val.p = NULL;
  return result;
}

/** Constructs an integer RaVar. */
RA_FUNC(RaVar) ra_int_new(RaI32 i) {
  RaVar          result;
  result.type  = RA_TYPE_INT;
  result.val.i = i;
  return result;
}

/** Constructs an numerical RaVar. */
RA_FUNC(RaVar) ra_num_new(RaF64 f) {
  RaVar          result;
  result.type  = RA_TYPE_NUMBER;
  result.val.n = f;
  return result;
}


