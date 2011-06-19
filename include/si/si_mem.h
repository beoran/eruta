#ifndef SI_MEM_H
#define SI_MEM_H

#include <stdlib.h>
#include "si_macro.h"


/** Allocate memory */
SI_API void * si_malloc(size_t size);

/** Free memory. */
SI_API void * si_free(void * ptr);
 
/** Copy memory. */ 
SI_API void * si_memcpy(void * dst, void * src, size_t size);

/** Re allocate memory. */
SI_API void * si_realloc(void * ptr, size_t size);

/* Safely copies memory between src and dst. dsts and srcs are the sizes of 
src and dst, dsti and srci are the indexes at chich to start copying, 
amount s the amount of data to copy.  */
SI_API void * si_smemcpy(void * dst, size_t dsts, size_t dsti, 
                  void * src, size_t srcs, size_t srci, 
                  size_t amount);

/** Allocate and copy memory. */
SI_API void * si_copyalloc(size_t size, void * src,  size_t tocopy);


/** Shorthand for si_malloc(sizof(STRUCTNAME)) */
#define SI_ALLOCATE(STRUCTNAME) si_malloc(sizeof(STRUCTNAME))

/** Shorthand for STRUCTNAME * FUNCNAME() { return si_malloc(sizof(STRUCTNAME)); } */
#define SI_DEFINE_ALLOCATE(STRUCTNAME, FUNCNAME) \
  STRUCTNAME * FUNCNAME() { return (STRUCTNAME *) si_malloc(sizof(STRUCTNAME));}
#endif
