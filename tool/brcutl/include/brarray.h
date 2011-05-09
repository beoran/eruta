
#ifndef _BRCUTL_ARRAY_H_
#define _BRCUTL_ARRAY_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "rtcutl.h"


/** BrArray is a generic array type that uses a preallocated buffer.
* All elements must be of a predefined size.  
*/
struct BrArray_;
typedef struct BrArray_ BrArray;

struct BrArray_ {
  uint8_t *ptr;
  size_t   len;
  size_t   cap;
  size_t   esz;
};

/**
* Initializes an empty array, using the given buffer ptr as storage.
* ptr points to an area of memory that has at least a capacity to store
* cap items of esz size each.
*/
BrArray * rut_array_init(BrArray * array, void * ptr, size_t cap, size_t esz);

/** Returns the length of the array.  */
size_t rut_array_len(BrArray * array);

/** Returns the capacity of the array.  */
size_t rut_array_cap(BrArray * array);

/** Returns the pointer of the array as a (void *) pointer */
void * rut_array_ptr(BrArray * array);

/** Returns the element size capacity of the array.  */
size_t rut_array_esz(BrArray * arr);


/** Returns a pointer offset, taking element size into consideration */
uint8_t * rut_array_offset(BrArray * array, uint8_t * ptr, int index); 

/** 
* Copies len elements from ptr into the array, starting from 
* pstart, and astart as long as there is enough capacity in the array 
* to do so, and if it would not meean exeeding plen 
* do so.
*/
BrArray * rut_array_xmemcpy(BrArray *arr, const void * ptr, size_t len, 
                      int astart, int pstart, size_t plen); 
/**
* Copies a C array from a memory location into the BrArray.
*/
BrArray * rut_array_memcpy(BrArray * arr, const void * ptr, size_t plen); 


/** Sets a value in the array at the given index.
* Returns null if it is out of range, returns array of all was ok. 
*/
BrArray * rut_array_set(BrArray * array, int index, const void *item);

/** 
* Gets a value in the array at the given index, and stores it in 
* item.
* Returns null if it is out of range, returns array of all was ok. 
*/
BrArray * rut_array_get(BrArray * array, int index, void *item);


/** Truncates an array to the given length by resetting it's length.
* Does not affect capacity. 
*/
BrArray * rut_array_trunc(BrArray * array, int size);

/** Concatenates arrays together. dst will be modified if it's capacity 
* allows it. BrArrays must have same element size. 
*/
BrArray * rut_array_cat(BrArray * dst, BrArray * src);

/** 
* Fills an array upto it's capacity with the given pointed to value.
* The array's length will be set 
*/
BrArray * rut_array_fill(BrArray * array, void * item);

/** Applies qsort in place to this array. */
BrArray * rut_array_qsort(BrArray * arr, BrCompare * compare);

#ifdef _DO_NOT_COMPILE_
/** Function type for rut_array_each. Return NULL to stop iteration. */
void * BrArrayIterator(BrArray * arr, size_t key, void * value, void * data);
 
/** Calls the iterator for each element. Returns the last value returnded by the iterator. Returns NULL if iteration was cancelled. */
void * rut_array_each(BrArray * arr, BrArrayIterator * iter, void * data); 
#endif

/** Prints out the array to FILE using the given format string. Nice for debugging.*/
BrArray * rut_array_printf(BrArray * arr, FILE * f, char * format);


BrArray * rut_array_iprintf(BrArray * arr, FILE * f);

#endif

