#ifndef ERUTA_MEM_H
#define ERUTA_MEM_H

#include "eruta.h"

/** Wrapper for calloc/malloc */
void * mem_calloc(size_t amount, size_t size);

/** Wrapper for calloc/malloc */
void * mem_alloc(size_t size);

/** Wrapper for free */
void * mem_free(void * ptr);

/** Wrapper for realloc */
void * mem_realloc(void *ptr, size_t newsize);

/** Resizes memory, taking care not to lose ptr* if the reallocation
fails. */
void * mem_resize(void ** ptr, size_t newsize);

/** Wrapper for memmove, for consistency */
void * mem_move(void * dest, void * src, size_t size);

/** A function pointer that can act as a destructor. Sould return NULL on 
sucessful freeing, and non-null if freeing failed. */
typedef void * (MemDestructor)(void * ptr);




/** Handy macros. */
/*
#define STRUCT_NALLOC(STRUCT, AMOUNT) \
        ((STRUCT *) mem_calloc(sizeof(STRUCT), (AMOUNT)))
*/

#define MEM_ALLOCATE(STRUCTNAME) mem_alloc(sizeof(STRUCTNAME))

#define STRUCT_NALLOC(STRUCT, AMOUNT) \
        (mem_calloc(sizeof(STRUCT), (AMOUNT)))

       
#define STRUCT_ALLOC(STRUCT)          \
        ((STRUCT *)MEM_ALLOCATE(STRUCT))
        
#define STRUCT_FREE(VAR)              \
        mem_free(VAR)


#endif