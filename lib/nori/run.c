
/**
* NoriRun is the run that supports norinning a nori script.
*/

#define NORI_INTERN_ONLY
#include "nori_intern.h"


/* Macros to make oop-style calls easier in handwritten code. */
#define NORI_DO(OBJ, FUNC, ...) OBJ->FUNC(OBJ, __VA_ARGS__)
#define NORI_STACKALLOC(PTR, SIZE) uint8_t PTR[SIZE]

struct NoriRun_;

typedef struct NoriRun_ NoriRun;

NORI_FUNCTYPE(NoriRun *) (*NoriFreeFunc) (NoriRun *self, void *   pointer);
NORI_FUNCTYPE(void    *) (*NoriAllocFunc)(NoriRun *self, NoriSize size);
NORI_FUNCTYPE(void    *) (*NoriReallocFunc)(NoriRun *self, NoriSize size);
 

// NoriRunto contains information of objects at run, so the object
// Does not have to track it itself.
struct NoriRunInfo_; 
typedef struct NoriRunInfo_ NoriRunInfo;

struct NoriRun_ {
  NoriAllocFunc   alloc;
  NoriFreeFunc    free;
  NoriReallocFunc resize;
};


struct NoriBase_;

typedef struct NoriBase_ NoriBase;

NORI_FUNCTYPE(NoriBase *) (*NoriBaseFreeFunc) (NoriBase *self);

typedef int32_t NoriUsing;
typedef int32_t NoriBaseFlags;

/* A base Object */
struct NoriBase_ {
  NoriRun          * run;
  NoriBaseFreeFunc   free;
  NoriSize           size;
  NoriBase         * owner;
  // if size == 0 , the object was allocated from the stack or externally 
  // and needs not be freed
  NoriUsing          use; 
  // how many times this object is used, AKA refcount
  // Reference count
};


NORI_FUNC(NoriRun*) nori_free(NoriRun * self , void * pointer) {
  if (!self)    return self;
  if (!pointer) return self;
  NORI_FREE(pointer);
  return self;
}


NORI_FUNC(void*)  nori_alloc(NoriRun * self, NoriSize size) {
  if (!self)    return NULL;
  return        NORI_MALLOC(size);  
}




/* Initializes a run with the required parameters. */ 
NORI_FUNC(NoriRun *) nori_run_init(NoriRun * self,
                                     NoriAllocFunc a, 
                                     NoriFreeFunc  f) {
  if (!self)    return self;
  self->alloc = ( a ? a : nori_alloc);
  self->free  = ( f ? f : nori_free );
  return self;
}


// Increases the reference count of a base object and returns it.
NORI_FUNC(NoriBase *)  nori_use(NoriBase * self) {
  // Null objects are passed as is.
  if (!self) return self;
  // Objects with no free function do not participate in ref counting 
  if (self->free == 0) return self;
  // increase ref and return 
  self->use++;
  return self;
}


// Decreases a reference counf of a base object, possibly destroying it.
// Returns NULL.
NORI_FUNC(NoriBase *)  nori_toss(NoriBase * self) {
  // Null objects are passed as is.
  if (!self)           return NULL;
  // Objects with no free function do not participate in ref counting 
  if (self->free == 0) return NULL;
  // Don't do anything if use is already 0
  if (self->use > 0) { 
    self->use--;
    if (self->use == 0) {
      self->free(self);
    }  
  }
  return NULL;
}

// The default runtime and a pointer to it
static NoriRun   default_run      = { nori_alloc, nori_free };
static NoriRun * default_run_ptr  = &default_run;

/* Sets the default run. Returns last default run. NOT REENTRANT! */
NORI_FUNC(NoriRun *) nori_run_default_set(NoriRun * self) {
  NoriRun * old_run = default_run_ptr;
  default_run_ptr     = self;
  return  old_run;
}

/* Gets the default run. Returns last default run. NOT REENTRANT! */
NORI_FUNC(NoriRun *) nori_run_default_get(NoriRun * self) {
  return  default_run_ptr;
}


// Frees a basic object only
NORI_FUNC(NoriBase *)  nori_base_free(NoriBase * b) {
  if (!b)            return b;
  if (!b->run)       return b;
  if (!b->run->free) return b;
  NORI_DO(b->run, free, b);
  return NULL;
}


// Initializes a basic object.
NORI_FUNC(NoriBase *)  nori_base_init(NoriBase * b, NoriRun * r,
        NoriBaseFreeFunc f, NoriSize s) {
  if (!b) return b;
  b->run        = r;
  b->free       = ( f ? f : nori_base_free);
  b->size       = s;
  b->use        = 1;
  return b;
}


// Makes a basic object, allocating it.
NORI_FUNC(NoriBase *) nori_base_make(NoriRun * r, NoriBaseFreeFunc f, NoriSize s) {
  NoriBase * b;
  b = (NoriBase *) NORI_DO(r, alloc, s);
  return nori_base_init(b, r, f, s);
}



