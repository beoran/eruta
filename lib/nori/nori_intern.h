/*
This file is for internal use by Nori library ONLY. Don't use anything
you find in here outside of the library. 
*/
#ifndef _NORI_INTERN_H_
#define _NORI_INTERN_H_


#ifndef NORI_INTERN_ONLY
#error This include file is only for used inside the Nori Library source files.
#endif

#include <stdint.h>

/* A few macros for memory allocation, so we can allow libgc if needed. */
#ifndef NORI_MALLOC
#define NORI_MALLOC(SIZE) (malloc(SIZE))
#endif 

#ifndef NORI_FREE
#define NORI_FREE(PTR) (free(PTR))
#endif 


#define NORI_ALLOCATE(TYPENAME) NORI_MALLOC(sizeof(TYPENAME))

#ifndef FALSE
#define FALSE (0)
#endif


#ifndef TRUE
#define TRUE (!FALSE)
#endif



#endif