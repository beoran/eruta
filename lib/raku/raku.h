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

/** Allot (allocate) memory block of the given size. May return null if the underlying system call fails. */
RA_FUNC(void *) ra_mem_allot(RaSize size); 

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

/** Constructs a RaVar that signals failure. 
* Used a bit like in Snobol or Icon. 
*/
RaVar ra_fail_new(); 

#define RA_FAILIURE (ra_fail_new)

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
#define RA_USE(OBJECT)  (ra_object_use((RaObject)(OBJECT)))
    
/** Decrease refcount, possibly calling the destructor if needed. */
#define RA_TOSS(OBJECT) (ra_object_toss((RaObject)(OBJECT)))


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
RA_FUNC(RaString) ra_string_free(RaString string) {
  ra_mem_free(ra_string_done(string));
}


/** Destructively resizes the RaString str to size sz. */
RA_FUNC(RaString) ra_string_newsize(RaString str, RaSize sz) {
  char * aid;
  if(!str) return NULL;
  aid           = ra_mem_allot(sz + 1); // One more for \0.
  if(!aid) return NULL;                 // Fail if no memory allocated.  
  ra_string_done(str);                  // Only now, free up old string contents.
  str->data     = aid;                  // Point to newly allocated string.      
  str->size     = sz;                   // Set size.   
  str->data[sz] = 0;                    // Nul terminate.
  return str;
}

/** Initializes an already allocated RaString. */
RA_FUNC(RaString) ra_string_init(RaString str, char * data, RaSize size) {
  if(!ra_string_newsize(str, size)) {
    return NULL;
  }   
  ra_strncpy(str->data, data, size);  
  return str;
}

RA_FUNC(RaString) ra_string_new(char * data, RaSize size) {
  RaString string = ra_mem_allot(sizeof(RaString));
  if(!string) return NULL;
  if(!ra_string_init(string, data, size)) {
    free(string); 
    return NULL;
  }
  return string;
}

RA_FUNC(RaString) ra_string_fromc(char * data) {
  RaSize size = strlen(data);
  return ra_string_new(data, size);
}

// The maximum number of significant (decimal!) figures in an IEEE
// double.
#ifndef RA_DOUBLE_DIGITS
#define RA_DOUBLE_DIGITS 16
#endif

RA_FUNC(RaSize) ra_string_size(RaString str) {
  if(!str) return 0; 
  return str->size;
}

/** Returns the char * representation \0 terminated, of the RaString. */
RA_FUNC(char *) ra_string_data(RaString str) {
  if(!str) return NULL;
  return (char*)str->data;
}

/** Puts the RaString to stdout. Useful for debugging. */
RA_FUNC(int) ra_string_puts(RaString str) {
  puts(ra_string_data(str));
}

RA_FUNC(RaString) ra_string_concat(RaString dest, RaString s1, RaString s2) {
  RaSize newsize;
  if(!dest || !s1 || !s2) return NULL;
  newsize = s1->size + s2->size + 1;
  ra_string_newsize(dest, newsize);
  ra_strncpy(dest->data , s1->data, s1->size);
  ra_strncpy(dest->data + s1->size, s2->data, s2->size);
  // Already \0 terminated by ra_string_newsize. 
  return dest;
}

/**Copies a substring between start and stop from src into dst if it can be 
   done so safely. */
char * ra_strbetween(char * dst, RaSize size, char * src, 
                 RaSize start, RaSize stop) {   
  int index;
  if (start > size)          return NULL; // can't copy past end of string.
  // Truncate stop that goes too far
  if (stop > size)    { stop = size ;  }    
  for(index = start; index < stop; index ++) {
    dst[index] = src[index]; 
  } 
  return dst;
}

RA_FUNC(RaString) ra_string_between(RaString dst, RaString str, 
                          RaSize start, RaSize stop) {
  RaSize newsize; 
  if(!dst || !str) return NULL;
  newsize = stop - start + 1;
  if (newsize < 0) { newsize = 0; }
  ra_string_newsize(dst, newsize);
  ra_strbetween(dst->data, dst->size, str->data, start, stop);
  return dst;  
}


RA_FUNC(int) ra_string_equal(RaString s1, RaString s2) {
  if(!s1 || !s2)            return FALSE;
  if(s1->size != s2->size)  return FALSE;
  return (memcmp(s1->data, s2->data, s1->size) == 0);
}

RA_FUNC(RaString) ra_string_fromnum(RaString dst, RaF64 num) {
  RaSize size;
  char buffer[1024]; // hope it's large enough :p
  if(!dst) return NULL; 
  sprintf(buffer, "%lf", num); // hope sprintf is accurate enough.
  size = strlen(buffer);
  ra_string_newsize(dst, size);
  ra_strncpy(dst->data, buffer, size);
  return dst;
}

  
RA_FUNC(int) ra_string_tof64(RaString src, RaF64 * f64) {
  if(!src || !src->data || !f64) return FALSE;
  (*f64) = atof(src->data);
  return TRUE;
}

RA_FUNC(int) ra_string_toi32(RaString src, RaI32 * i32) {
  if(!src || !src->data || !i32) return FALSE;
  (*i32)  =  atol(src->data);
  return TRUE;
}




#endif
