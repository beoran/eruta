#ifndef _NORI_H_
#define _NORI_H_

/* 
  Some platforms will need a __declspec(dllexport) or something for 
  functions in a shared library.
*/
#ifndef NORI_EXPORT_FUNC
#define NORI_EXPORT_FUNC extern
#endif
  
/*
* Some platforms may require a change in calling convention
*/  
#ifndef NORI_CALL_FUNC
#define NORI_CALL_FUNC
#endif 

/* All in a handy wrapper macro */
#define NORI_FUNC(RESULT) NORI_EXPORT_FUNC RESULT NORI_CALL_FUNC
 
/* An another wrapper macro to help typedefing such functions */
#define NORI_FUNCTYPE(RESULT) typedef RESULT NORI_CALL_FUNC



// bag, bin, bar, row, box, jar, keg, kit, pea, pod, pot,rig, can, tin, tub, wad

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// Some typedefs for sizes, integers, doubles, etc. 
typedef size_t 	 NoriSize;
typedef uint32_t NoriU32;
typedef int32_t  NoriI32;
typedef double   NoriF64;
typedef uint8_t  NoriByte;
typedef void *   NoriPtr;


/** A noriCurator takes care of memory management and allocation. */
struct NoriCurator_;
typedef struct NoriCurator_ NoriCurator;

/** Allocator function type. */
NORI_FUNCTYPE(void *) NoriCuratorAllocate(NoriCurator * c, NoriSize size);
/** Memory block resize function type. */
NORI_FUNCTYPE(void *) NoriCuratorResize(NoriCurator * c, void * mem, NoriSize size);
/** Memory block free function type. */
NORI_FUNCTYPE(void)   NoriCuratorFree(NoriCurator * c, void * mem);

struct NoriCurator_ {
  NoriCuratorAllocate * allocate;
  NoriCuratorResize   * resize;
  NoriCuratorFree     * free;
};

/** Built in wrappers for malloc, realloc and free */
NORI_FUNC(void *) nori_curator_malloc(NoriCurator * c, NoriSize size);
NORI_FUNC(void *) nori_curator_realloc(NoriCurator * c, void * mem, NoriSize size);
NORI_FUNC(void)   nori_curator_freealloc(NoriCurator * c, void * mem); 

/** Initializes a custom curator. */
NORI_FUNC(NoriCurator *) nori_curator_init(NoriCurator * c,
  NoriCuratorAllocate * a, NoriCuratorResize * r, NoriCuratorFree * f); 

/** Returns the default global curator. */
NORI_FUNC(NoriCurator *) nori_curator(); 

/** 
Sets the default global custom curator. 
Call this as soon as possible after startup. 
*/
NORI_FUNC(void)   nori_curator_set(NoriCurator * c); 

/** Allocate memory using a curator. */
NORI_FUNC(void *) nori_curated_allocate(NoriCurator * c, NoriSize size); 

/** Resize memory using a curator. */
NORI_FUNC(void *) nori_curated_resize(NoriCurator * c, void * mem, NoriSize size);

/** Free memory using a curator. */
NORI_FUNC(void)   nori_curated_free(NoriCurator * c, void * mem);

/** Allocate, resize and free memory, using the default curator. */
NORI_FUNC(void *) nori_allocate(NoriSize size); 
NORI_FUNC(void *) nori_resize(void * mem, NoriSize size); 
NORI_FUNC(void)   nori_free(void * mem);

/** Rows are generic low level arrays. */








#endif
