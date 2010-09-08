#include <stdlib.h>
#include <stdio.h>

#define NORI_INTERN_ONLY
#include "nori_intern.h"

/** Returns the length of the array.  */
size_t array_len(Array * array) {
  return array->len;
}

/** Returns the capacity of the array.  */
size_t array_cap(Array * array) {
  return array->cap;
}


/* Full in bytes of the buffer of the Array. */
size_t array_sizeof(Array * array) {
  return array->cap * array->esz;  
}

/* Returns the pointer of the array as a (void *) pointer. */
void * array_ptr(Array * array) {
  return array->ptr;
}


/**
* Initializes an empty array, using the given buffer ptr as storage.
* ptr points to an area of memory that has at least a capacity to store
* cap items of esz size each.
*/
Array * array_init(Array * array, void * ptr, size_t cap, size_t esz) { 
  array->ptr      = ptr;
  array->cap      = cap;
  array->esz      = esz;
  array->len      = 0;
  // Ensure buffer is zero filled.
  memset(array->ptr, 0, array_sizeof(array));  
  return array;
}


/** Returns a pointer offset, taking element size into consideration */
uint8_t * array_offset(Array * array, uint8_t * ptr, int offset) {
  return ptr + (offset * array->esz);
}


/** Checks if the arguments are ok or not */
int array_xmemcpy_ok(Array * arr, size_t len, 
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
    fprintf(stderr, "Array capacity too small.\n"); 
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
* to do so, and if it would not meean exeeding plen 
* do so.
*/
Array * array_xmemcpy(Array *arr, const void * ptr, size_t len, 
                      int astart, int pstart, size_t plen) {
  int index, offset, memdex;
  uint8_t * src, * dst;
  uint8_t * ptr8 = (uint8_t *) ptr;
  // Check arguments and return NULL if out of range.   
  if(!array_xmemcpy_ok(arr, len, astart, pstart, plen)) {
    return NULL;
  }

  src = array_offset(arr, ptr8    , pstart);
  dst = array_offset(arr, arr->ptr, astart);
  memmove(dst, src, arr->esz * len);
  // copy all the data in one block. 
  /*
  // copy as instructed
  for (index = 0; index < len; index ++) {
    dst = array_offset(arr, arr->ptr, (index + astart));
    src = array_offset(arr, ptr8    , (index + pstart)); 
    memcpy(dst, src, arr->esz);
  } 
  */
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
Array * array_xmemcpyto(Array *arr, void * ptr, size_t len, 
                      int astart, int pstart, size_t plen) {
  int index, offset, memdex;
  uint8_t * src, * dst;
  uint8_t * ptr8 = (uint8_t *) ptr;
  // Check arguments and return NULL if out of range.   
  if(!array_xmemcpy_ok(arr, len, astart, pstart, plen)) {
    return NULL;
  }
  src = array_offset(arr, ptr8    , pstart);
  dst = array_offset(arr, arr->ptr, astart);
  memmove(src, dst, arr->esz * len);
  // copy all the data in one block. 
  /*
  // copy as instructed
  for (index = 0; index < len; index ++) {
    dst = array_offset(arr, arr->ptr, (index + astart));
    src = array_offset(arr, ptr8    , (index + pstart)); 
    memcpy(dst, src, arr->esz);
  } 
  */
  return arr;
}  


/**
* Copies a C array from a memory location into the Array.
*/
Array * array_memcpy(Array * arr, const void * ptr, size_t plen) {
  return array_xmemcpy(arr, ptr, plen, 0, 0, plen);
}

/** 
* Sets a value in the array at the given index.
* Returns null if it is out of range, returns array of all was ok. 
*/
Array * array_set(Array * array, int index, const void *item) {
  // Copy the single item into the right place. 
  return array_xmemcpy(array, item, 1, index, 0, 1); 
}

/** 
* Gets a value in the array at the given index, and stores it in 
* item.
* Returns null if it is out of range, returns array of all was ok. 
*/
Array * array_get(Array * array, int index, void *item) {
  return array_xmemcpyto(array, item, 1, index, 0, 1);
} 

/** 
* Fills an array upto it's capacity with the given pointed to value.
* The array's length will be set 
*/
Array * array_fill(Array * array, void * item) {
  int index;
  for( index=0; index < array_cap(array); index++) {
    array_set(array, index, item);
  }
} 


/** Truncates an array to the given length by resetting it's length.
* Does not affect capacity. 
*/
Array * array_trunc(Array * array, int size) { 
  if (size < 0)           { return NULL; }
  if (size >= array->cap) { return NULL; }
  array->len = size; 
}


/** 
* Concatenates arrays together. dst will be modified if it's capacity 
* allows it. Arrays must have same element size. 
*/
Array * array_cat(Array * dst, Array * src) {
  if ( dst->esz != src->esz) {
    fprintf(stderr, "Element size differs!\n"); 
    return NULL; 
  }
  return array_xmemcpy(dst, array_ptr(src), array_len(src),
                       array_len(dst), 0, array_len(src));  
}





