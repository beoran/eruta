/**

Some ideas on memory managment. 

How can pointers be used: 

1) Returned from function.
2) Passed as argument to function.
3) Stored in a global variable.
4) Stored in a function argument passed.

Distinguish between owned and borrowed references? 



*/



#define NORI_INTERN_ONLY
#include "nori_intern.h"

/** 
Flexible memory management for C. 
To make it easy to dynamically change the way memory is allocated, 
as well as howit is tracked. */



/** The curatore takes care of, manages and acts upon the memory ysed by the 
* application. It may or may not support garbage collection.
*/

NORI_FUNC(void *) nori_curator_malloc(NoriCurator * c, NoriSize size) {
   return NORI_MALLOC(size); 
}

NORI_FUNC(void *) nori_curator_realloc(NoriCurator * c, void * mem, NoriSize size) {
   return NORI_REALLOC(mem, size); 
}

NORI_FUNC(void) nori_curator_freealloc(NoriCurator * c, void * mem) {
   return NORI_FREE(mem); 
}


NORI_FUNC(NoriCurator *) nori_curator_init(NoriCurator * c,
  NoriCuratorAllocate * a, NoriCuratorResize * r, NoriCuratorFree * f) {
  c->allocate = (a ? a : nori_curator_malloc);
  c->resize   = (r ? r : nori_curator_realloc);
  c->free     = (f ? f : nori_curator_freealloc);
  return c;
}

NoriCurator nori_global_curator_struct 	= { NULL, NULL, NULL };
NoriCurator * nori_global_curator 	= NULL;

NORI_FUNC(NoriCurator *) nori_curator() {
  if (!nori_global_curator) { 
    nori_global_curator = & nori_global_curator_struct;
    nori_curator_init(nori_global_curator, NULL, NULL, NULL);
  }
  return nori_global_curator;
}

NORI_FUNC(void) nori_curator_set(NoriCurator * c) {
  nori_global_curator = c;
}

NORI_FUNC(void *) nori_curated_allocate(NoriCurator * c, NoriSize size) {
  if((!c) || (!c->allocate)) { return nori_curator_malloc(c, size); }
  return c->allocate(c, size);
}

NORI_FUNC(void *) nori_curated_resize(NoriCurator * c, void * mem, NoriSize size) {
  if((!c) || (!c->resize)) { return nori_curator_realloc(c, mem, size); }
  return c->resize(c, mem, size);
}

NORI_FUNC(void) nori_curated_free(NoriCurator * c, void * mem) {
  if((!c) || (!c->free)) { return nori_curator_freealloc(c, mem); }
  c->free(c, mem);
}

NORI_FUNC(void *) nori_allocate(NoriSize size) {
  NoriCurator * c = nori_curator();
  return nori_curated_allocate(c, size);
} 

NORI_FUNC(void *) nori_resize(void * mem, NoriSize size) {
  NoriCurator * c = nori_curator();
  return nori_curated_resize(c, mem, size);
} 

NORI_FUNC(void) nori_free(void * mem) {
  NoriCurator * c = nori_curator();
  nori_curated_free(c, mem);
} 












