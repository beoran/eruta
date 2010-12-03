/** Generic array type. */

#include "raku.h"
#include <stdlib.h>
#include <string.h>

/** A array is an array. */

struct RaArrayStruct_ {
  RA_OBJECT_HEADER
  RaVar   * ptr;
  RaSize    size;
  RaSize    cap; // capacity
};


/** Returns the length of the array.  */
RA_FUNC(RaSize) ra_array_size(RaArray array) {
  return array->size;
}

/** Returns the capacity of the array.  */
RA_FUNC(RaSize) ra_array_cap(RaArray array) {
  return array->cap;
}


/* Full in bytes of the buffer of the RaArray. */
RaSize ra_array_sizeof(RaArray array) {
  return array->cap * sizeof(RaVar);  
}

/* Returns the pointer of the array as a (void *) pointer. */
void * ra_array_ptr(RaArray array) {
  return (void *)array->ptr;
}


/** Cleans up the contents of the array after use, although it does not call 
* any cleanup function on the elements themselves. 
*/
RA_FUNC(RaArray) ra_array_done(RaArray array) {
  int index;
  if(array && array->ptr) {
    for(index = 0; index < array->size; index++) {
      ra_var_toss(array->ptr[index]); // Clean up held references. 
    } 
    ra_mem_free(array->ptr);
    array->ptr = NULL;
    array->cap = 0;
    array->size= 0;
  } 
  return array;
}

/**
* Cleans up the arrray and it's contents, rteleasing all memory allocated.
*/
RA_FUNC(void) ra_array_free(RaArray array) {
  ra_array_done(array);
  ra_mem_free(array);
}


/**
* Resizes an array to have a new capacity of newcap. Keeps the elements intact.
* Never shrinks the array to size 0;  
*/
RA_FUNC(RaArray) ra_array_newcap(RaArray arr, RaSize newcap) {
  RaVar * aid;
  int index;
  
  if(!arr)        return NULL;
  if(newcap < 1)  return NULL;
  aid = ra_mem_resize(arr->ptr, newcap * sizeof(RaVar));
  if(!aid)        return NULL;
  arr->ptr      = aid;
  arr->cap      = newcap;
  
  for(index = 0; index < arr->size; index++) {
    arr->ptr[index] = RA_FAILURE; 
    // Set to no reference 
  } 

  
  return arr;
}


/**
* Initializes an empty array with zero capacity.
*/

RaArray ra_array_initempty(RaArray arr) {
  if (!arr) return NULL;
  arr->ptr  = NULL;
  arr->cap  = 0;
  arr->size = 0;
  return arr;  
}


/**
* Initializes an empty array by allocating enough space for cap RaVar elements.
*/
RA_FUNC(RaArray) ra_array_init(RaArray arr, RaSize cap) {
  if(!arr) return NULL;
  RA_OBJECT_INIT(arr, ra_array_free); 
  ra_array_initempty(arr);
  return ra_array_newcap(arr, cap);
}

/**
* Allocates a new array with a default capacity of 16 elements. 
*/
RA_FUNC(RaArray) ra_array_new() {
  RaArray arr = ra_mem_alloc(sizeof(RaArrayStruct));  
  return ra_array_init(arr, 16);
}


RA_FUNC(RaVar) ra_array_get(RaArray arr, RaSize index) {
  if(!arr || !arr->ptr) return ra_fail_new();
  if(index > arr->size) return ra_fail_new();
  return arr->ptr[index];  
}

RA_FUNC(RaVar) ra_array_set(RaArray arr, RaSize index, RaVar value) {
  if(!arr || !arr->ptr) return ra_fail_new();
  while(arr->cap < index) {
    if(!ra_array_newcap(arr, arr->cap * 2)) return ra_fail_new();    
  } 
  arr->ptr[index] = value;
  if(index > arr->size) 
  arr->size       = index + 1;
  return value;
}


RA_FUNC(RaVar) ra_array_push(RaArray arr, RaVar value) {
   return ra_array_set(arr, arr->size, value);
}

RA_FUNC(RaVar) ra_array_pop(RaArray arr) {
   if(arr->size < 1) return ra_fail_new();
   arr->size--;   
   return ra_array_get(arr, arr->size);
}

/*
naRef naVec_get(naRef v, int i)
void naVec_set(naRef vec, int i, naRef o);
int naVec_size(naRef v);
int naVec_append(naRef vec, naRef o);
void naVec_setsize(naRef vec, int sz);
naRef naVec_removelast(naRef vec);
*/


