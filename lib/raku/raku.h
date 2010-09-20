#ifndef _RA_H_
#define _RA_H_

/* 
  Some platforms will need a __declspec(dllexport) or something for 
  functions in a shared library.
*/
#ifndef RA_EXPORT_FUNC
#define RA_EXPORT_FUNC extern
#endif
  
/*
* Some platforms may require a change in calling convention
*/  
#ifndef RA_CALL_FUNC
#define RA_CALL_FUNC
#endif 

/* All in a handy wrapper macro */
#define RA_FUNC(RESULT) RA_EXPORT_FUNC RESULT RA_CALL_FUNC
 
/* An another wrapper macro to help typedefing such functions */
#define RA_FUNCTYPE(RESULT) typedef RESULT RA_CALL_FUNC



// bag, bin, bar, row, box, jar, keg, kit, pea, pod, pot,rig, can, tin, tub, wad

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// Some typedefs for sizes, integers, doubles, etc. 
typedef size_t 	 RaSize;
typedef uint32_t RaU32;
typedef int32_t  RaI32;
typedef double   RaF64;
typedef uint8_t  RaByte;
typedef uint8_t  RaU8;
typedef void *   RaPtr;

/* For refcounts. We use only 16 bits refcounts since it should be enough.
 objects that are referred more than 65000 times are considered to have become 
 "immortal". */
typedef uint16_t RaCount;
// For primitive object types 
typedef uint8_t  RaType;

enum RaTypeEnum_ {
  RA_TYPE_FAKE    =  0,
  RA_TYPE_FAIL    =  1, 
  RA_TYPE_INTEGER =  2, 
  RA_TYPE_NUMBER  =  3, 
  RA_TYPE_STRING  =  4, 
  RA_TYPE_ARRAY   =  5, 
  RA_TYPE_TABLE   =  6, 
  RA_TYPE_DEF     =  7,
  RA_TYPE_CODE    =  8,
  RA_TYPE_BOX     =  9,
  RA_TYPE_BRIDGE  = 10, 
};

/** Built in wrappers for handling memory, like 
malloc, realloc, zero, set, copy  and free */
RA_FUNC(void *) ra_mem_alloc(RaSize size);
RA_FUNC(void *) ra_mem_resize(void * mem, RaSize size);
RA_FUNC(void)   ra_mem_free(void   * mem);
RA_FUNC(void *) ra_mem_set(void    * dst, int c, RaSize size);
RA_FUNC(void *) ra_mem_zero(void   * dst);
RA_FUNC(void *) ra_mem_copy(void   * dst, void * src, RaSize size);

struct RaVar_ {
  union { 
    RaI32 integer; 
    RaF64 number;
    RaPtr pointer;
  } value;
  RaType    type;  // Object type.
};

typedef struct RaVar_ RaVar;

struct RaObject_ {
  RaCount count;
};

typedef struct RaCount_ RaCount;





#endif
