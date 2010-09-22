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

/* Of course we need some fake booleans. */
#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE  (!(FALSE))
#endif

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
  RA_TYPE_FAIL    =  0,
  RA_TYPE_OK      =  1, 
  RA_TYPE_INT     =  2, 
  RA_TYPE_NUMBER  =  3, 
  RA_TYPE_STRING  =  4, 
  RA_TYPE_ARRAY   =  5, 
  RA_TYPE_TAB     =  6, 
  RA_TYPE_DEF     =  7,
  RA_TYPE_CODE    =  8,
  RA_TYPE_BOX     =  9,
  RA_TYPE_BRIDGE  = 10, 
};

/** Built in wrappers for handling memory, like 
malloc, realloc, zero, set, copy  and free */

/** Allocate memory block of the given size. May return null if the underlying system call fails. */
RA_FUNC(void *) ra_mem_alloc(RaSize size); 

/** Frees the memory block. meme may also be NULL in which case nothing 
happens. */
RA_FUNC(void) ra_mem_free(void * mem);

/** Reallocates a memory area to have the given size.  */
RA_FUNC(void *) ra_mem_resize(void * mem, RaSize size);

/** Sets size bytes of the area poined to by dst to c. */
RA_FUNC(void *) ra_mem_set(void    * dst, int c, RaSize size); 

/** Zeroes size bytes of dst. */  
RA_FUNC(void *) ra_mem_zero(void   * dst, RaSize size);

/** Moves memory from src to dst. the areas may overlap. */
RA_FUNC(void *) ra_mem_move(void   * dst, const void * src, RaSize size); 


struct RaVar_ {
  union { 
    RaI32 i; 
    RaF64 n;
    RaPtr p;
  } val;
  RaType    type;  // Object type.
};

typedef struct RaVar_ RaVar;

/** Constructs a RaVar that signals failure. Used like in Icon. */
RA_FUNC(RaVar) ra_fail_new(); 

/** Constructs an integer RaVar. */
RA_FUNC(RaVar) ra_int_new(RaI32 i);
 
/** Constructs an numerical RaVar. */
RA_FUNC(RaVar) ra_num_new(RaF64 f);

#define RA_FAILURE (ra_fail_new())

/** Macro to construct a numerical RaVar */
#define RA_NUM(N) ra_num_new(N)
/** Macro to construct an integer RaVar */
#define RA_INT(I) ra_int_new(I)


/** Increases the ref count on an object contained in a ra_var if 
it contains a pointer. */
RA_FUNC(RaVar) ra_var_use(RaVar var); 

/** Decreases the ref count on an object contained in a ra_var if 
it contains a pointer. */
RA_FUNC(RaVar) ra_var_toss(RaVar var);

#define RA_USE(VAR)  ra_var_use(&VAR)
#define RA_TOSS(VAR) ra_var_toss(&VAR)


/** 
The object header that every built-in Raku object must have.
Consists of a refcount and a destructor function. 
*/

RA_FUNCTYPE(void) (* RaDestructor)(RaPtr pointer);

#define RA_OBJECT_HEADER    \
  RaCount         count_;   \
  RaDestructor    free_;


/** The generic raku object struct. */
struct RaObjectStruct_ {
  RA_OBJECT_HEADER
};

typedef struct RaObjectStruct_ RaObjectStruct;
typedef RaObjectStruct * RaObject;


/** Allot (allocate) memory block of the given size. May return null if the underlying system call fails. */
RA_FUNC(void *) ra_mem_allot(RaSize size); 

/** Frees the memory block. meme may also be NULL in which case nothing 
happens. */
RA_FUNC(void) ra_mem_free(void * mem); 


/** Generic Raku object initialization function. */
RA_FUNC(RaObject) ra_object_init(RaObject obj, RaDestructor des); 

/** Generic Raku object initialization mcro. */
#define RA_OBJECT_INIT(OBJECT, FREE)  (ra_object_init((RaObject)(OBJECT), (RaDestructor)(FREE)))


/** Generic Raku object finalization function. */
RA_FUNC(void) ra_object_free(RaObject obj);


/** Increase refcount, taking care not to exceed the max.  
Returns the object, or NULL if it was already null.
*/
RA_FUNC(RaObject) ra_object_use(RaObject obj); 
  
/** Decrease refcount, possibly calling the destructor if the refcount == 0.
Returns the object, or NULL if it was destroyed. 
**/
RA_FUNC(RaObject) ra_object_toss(RaObject obj);

/** Increase refcount, taking care not to exceed the max.  */
#define RA_OBJECT_USE(OBJECT)  (ra_object_use((RaObject)(OBJECT)))
    
/** Decrease refcount, possibly calling the destructor if needed. */
#define RA_OBJECT_TOSS(OBJECT) (ra_object_toss((RaObject)(OBJECT)))


/** String functions. */

struct RaStringStruct_;

typedef struct RaStringStruct_ RaStringStruct;
typedef RaStringStruct       * RaString;
typedef const RaStringStruct * RaConstString;

struct RaStringStruct_ {
  RA_OBJECT_HEADER
  char    * data;
  RaSize    size;
  RaSize    hash;
};


/** Frees data associated with the rastring and set it's size to 0 
and it's data to NULL. */
RA_FUNC(RaString) ra_string_done(RaString string); 

/** Frees the RaString as a whole. */
RA_FUNC(RaString) ra_string_free(RaString string);

/** Destructively resizes the RaString str to size sz. */
RA_FUNC(RaString) ra_string_newsize(RaString str, RaSize sz);

/** Initializes an already allocated RaString. */
RA_FUNC(RaString) ra_string_init(RaString str, char * data, RaSize size);

RA_FUNC(RaString) ra_string_new(char * data, RaSize size);


RA_FUNC(RaString) ra_string_fromc(char * data); 

RA_FUNC(RaSize) ra_string_size(RaString str);

/** Returns the char * representation \0 terminated, of the RaString. */
RA_FUNC(char *) ra_string_data(RaString str);

/** Puts the RaString to stdout. Useful for debugging. */
RA_FUNC(int) ra_string_puts(RaString str); 

RA_FUNC(RaString) ra_string_concat(RaString dest, RaString s1, RaString s2); 

RA_FUNC(RaString) ra_string_between(RaString dst, RaString str, 
                          RaSize start, RaSize stop);
                          
RA_FUNC(RaString) ra_string_slice(RaString dst, RaString str, 
                          RaSize start, RaSize count);

RA_FUNC(int) ra_string_equal(RaString s1, RaString s2);

RA_FUNC(RaString) ra_string_fromf64(RaString dst, RaF64 num);
RA_FUNC(RaString) ra_string_fromi32(RaString dst, RaI32 num);

RA_FUNC(int) ra_string_tof64(RaString src, RaF64 * f64);

RA_FUNC(int) ra_string_toi32(RaString src, RaI32 * i32);



/** Array functions. */
/** A array is an array. */
struct RaArray_;
typedef struct RaArrayStruct_ RaArrayStruct;
typedef RaArrayStruct * RaArray;

/** Returns the length of the array.  */
RA_FUNC(RaSize) ra_array_size(RaArray array);

/** Returns the capacity of the array.  */
RA_FUNC(RaSize) ra_array_cap(RaArray array);

/** Returns the pointer of the array as a (void *) pointer. */
RA_FUNC(void *) ra_array_ptr(RaArray array);

/** Cleans up the contents of the array after use, although it does not call 
* any cleanup function on the elements themselves. 
*/
RA_FUNC(RaArray) ra_array_done(RaArray array);

/** Cleans up the arrray and it's contents, releasing all memory allocated. */
RA_FUNC(void) ra_array_free(RaArray array); 

/** Resizes an array to have a new capacity of newcap. Keeps the elements 
* intact. Never shrinks the array to size 0.  
*/
RA_FUNC(RaArray) ra_array_newcap(RaArray arr, RaSize newcap);

/** Initializes an empty array by allocating enough space for cap RaVar 
* elements.
*/
RA_FUNC(RaArray) ra_array_init(RaArray arr, RaSize cap); 

/** Allocates a new array with a default capacity of 16 elements. */
RA_FUNC(RaArray) ra_array_new();

/** Gets a value from the array. */
RA_FUNC(RaVar) ra_array_get(RaArray arr, RaSize index); 

/** Sets a value into the array. */
RA_FUNC(RaVar) ra_array_set(RaArray arr, RaSize index, RaVar value); 

/** Adds an element at the end of the array.*/
RA_FUNC(RaVar) ra_array_push(RaArray arr, RaVar value); 

/** Removes an element from the end of the array. */
RA_FUNC(RaVar) ra_array_pop(RaArray arr);




#endif
