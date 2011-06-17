#ifndef RE_MEM_H
#define RE_MEM_H

#include <stdlib.h>

/** Allocate memory */
void * re_malloc(size_t size);

/** Free memory. */
void * re_free(void * ptr);
 
/** Copy memory. */ 
void * re_memcpy(void * dst, void * src, size_t size);

/** Re allocate memory. */
void * re_realloc(void * ptr, size_t size);

/* Safely copies memory between src and dst. dsts and srcs are the sizes of 
src and dst, dsti and srci are the indexes at chich to start copying, 
amount s the amount of data to copy.  */
void * re_smemcpy(void * dst, size_t dsts, size_t dsti, 
                  void * src, size_t srcs, size_t srci, 
                  size_t amount);

/** Allocate and copy memory. */
void * re_copyalloc(size_t size, void * src,  size_t tocopy);



#endif
