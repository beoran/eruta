#include <stdlib.h>
#include <stdio.h>

#define NORI_INTERN_ONLY
#include "nori_intern.h"

struct NoriArray_ {
  void * ptr;
  size_t len;
  size_t cap;
  size_t esz;
};

typedef struct NoriArray_ NoriArray;

/** Returns the length of the array.  */
size_t nori_array_len(NoriArray * array) {
  return array->len;
}

/** Returns the capacity of the array.  */
size_t nori_array_cap(NoriArray * array) {
  return array->cap;
}


/* Full in bytes of the buffer of the NoriArray. */
size_t nori_array_sizeof(NoriArray * array) {
  return array->cap * array->esz;  
}

/* Returns the pointer of the array as a (void *) pointer. */
void * nori_array_ptr(NoriArray * array) {
  return array->ptr;
}


/**
* Initializes an empty array, using the given buffer ptr as storage.
* ptr must point to an area of memory that has at least a capacity to store
* cap items of esz size each.
*/
NoriArray * nori_array_initptr
            (NoriArray * array, void * ptr, size_t cap, size_t esz) { 
  array->ptr      = ptr;
  array->cap      = cap;
  array->esz      = esz;
  array->len      = 0;
  // Ensure buffer is zero filled.
  memset(array->ptr, 0, nori_array_sizeof(array));  
  return array;
}

/**
* Initializes an empty array by allocating enough space for cap 
* elements of size esz each.
*/
NoriArray * nori_array_init(NoriArray * array, size_t cap, size_t esz) { 
  void * ptr      = NORI_MALLOC(cap * esz); 
  return nori_array_initptr(array, ptr, cap, esz);
}


/** Returns a pointer offset, taking element size into consideration */
uint8_t * nori_array_offset(NoriArray * array, uint8_t * ptr, int offset) {
  return ptr + (offset * array->esz);
}

/** Checks if the arguments are ok or not */
int nori_array_xmemcpy_ok(NoriArray * arr, size_t len, 
                      int astart, int pstart, size_t plen) { 
  
  if (astart < 0)                 { 
    fprintf(stderr, "Astart negative\n"); return FALSE; 
  } 
  if (pstart < 0)                 { 
    fprintf(stderr, "Pstart negative\n"); 
    return FALSE; 
  }
  if (len    < 0)                 { 
    fprintf(stderr, "Len negative\n");
    return FALSE; 
  }
  if ((astart + len) > arr->cap)  {
    fprintf(stderr, "NoriArray capacity too small.\n"); 
    return FALSE; 
  }
  if ((pstart + len) >  plen)     {
    fprintf(stderr, "Plen exeeded.\n"); 
    return FALSE; 
  }
  return TRUE;
}

/** 
* Copies len elements from ptr into the array, starting from 
* pstart, and astart as long as there is enough capacity in the array 
* to do so, and if it would not mean exeeding plen to do so.
*/
NoriArray * nori_array_xmemcpy(NoriArray *arr, const void * ptr, size_t len, 
                      int astart, int pstart, size_t plen) {
  int index, offset, memdex;
  uint8_t * src, * dst;
  uint8_t * ptr8 = (uint8_t *) ptr;
  // Check arguments and return NULL if out of range.   
  if(!nori_array_xmemcpy_ok(arr, len, astart, pstart, plen)) {
    return NULL;
  }

  src = nori_array_offset(arr, ptr8    , pstart);
  dst = nori_array_offset(arr, arr->ptr, astart);
  memmove(dst, src, arr->esz * len);
  // copy all the data in one block. 
  // set array length based on parameters
  if ( (len + astart ) > arr->len) { 
    arr->len  = (len + astart );
  }  
  return arr;
}  

/** 
* Copies len elements from array into the ptr, starting from 
* astart, and pstart as long as there is enough capacity in the array 
* to do so, and if it would not mean exeeding plen 
* do so.
*/
NoriArray * nori_array_xmemcpyto(NoriArray *arr, void * ptr, size_t len, 
                      int astart, int pstart, size_t plen) {
  int index, offset, memdex;
  uint8_t * src, * dst;
  uint8_t * ptr8 = (uint8_t *) ptr;
  // Check arguments and return NULL if out of range.   
  if(!nori_array_xmemcpy_ok(arr, len, astart, pstart, plen)) {
    return NULL;
  }
  src = nori_array_offset(arr, ptr8    , pstart);
  dst = nori_array_offset(arr, arr->ptr, astart);
  memmove(src, dst, arr->esz * len);
  return arr;
}  


/**
* Copies a C array from a memory location into the NoriArray.
*/
NoriArray * nori_array_memcpy(NoriArray * arr, const void * ptr, size_t plen) {
  return nori_array_xmemcpy(arr, ptr, plen, 0, 0, plen);
}

/** 
* Sets a value in the array at the given index.
* Returns null if it is out of range, returns array of all was ok. 
*/
NoriArray * nori_array_set(NoriArray * array, int index, const void *item) {
  // Copy the single item into the right place. 
  return nori_array_xmemcpy(array, item, 1, index, 0, 1); 
}

/** 
* Gets a value in the array at the given index, and stores it in 
* item.
* Returns null if it is out of range, returns array of all was ok. 
*/
NoriArray * nori_array_get(NoriArray * array, int index, void *item) {
  return nori_array_xmemcpyto(array, item, 1, index, 0, 1);
} 

/** 
* Fills an array upto it's capacity with the given pointed to value.
* The array's length will be set 
*/
NoriArray * nori_array_fill(NoriArray * array, void * item) {
  int index;
  for( index=0; index < nori_array_cap(array); index++) {
    nori_array_set(array, index, item);
  }
} 


/** Truncates an array to the given length by resetting it's length.
* Does not affect capacity. 
*/
NoriArray * nori_array_trunc(NoriArray * array, int size) { 
  if (size < 0)           { return NULL; }
  if (size >= array->cap) { return NULL; }
  array->len = size; 
}


/** 
* Concatenates arrays together. dst will be modified if it's capacity 
* allows it. NoriArrays must have same element size. 
*/
NoriArray * nori_array_cat(NoriArray * dst, NoriArray * src) {
  if (dst->esz != src->esz) {
    fprintf(stderr, "Element size differs!\n"); 
    return NULL; 
  }
  return nori_array_xmemcpy(dst, nori_array_ptr(src), nori_array_len(src),
                       nori_array_len(dst), 0, nori_array_len(src));  
}





