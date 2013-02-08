#include <stdlib.h>

#include "tbegin.h"

/*
* TARRAY is template for a dynamic array of size elements.
* For simplicity, it is is exactly sized, that is,
* it does not allocate any extra elements but the amount requested.
* In other words it's capacity is equal to it's size.
*/
#ifdef TEMPLATE_OK

#ifndef TEMPLATE_IMPLEMENT

struct TEMPLATE_STRUCT;

typedef struct TEMPLATE_STRUCT TEMPLATE_NAME;

#define TEMPLATE_COMPARATOR TJOIN(TEMPLATE_STRUCT, Compare)

typedef int TEMPLATE_COMPARATOR (const void * self, const void * other);

  

/* Gets the array's size. Returns 0 if self is NULL. */
int TEMPLATE_FUNC(size)(TEMPLATE_NAME * self);

/* Frees the contents of an array. Has the same effect as emptying the array. 
Does not call a destructor on any elements contained! */
TEMPLATE_NAME * TEMPLATE_FUNC(done)(TEMPLATE_NAME * self);

/* Frees an array. Returns NULL. */
TEMPLATE_NAME * TEMPLATE_FUNC(free)(TEMPLATE_NAME * self);

/* Allocates a new unusable array.  */
TEMPLATE_NAME * TEMPLATE_FUNC(alloc)();

/* Initializes an empty array with size elements. */
TEMPLATE_NAME * TEMPLATE_FUNC(init)(TEMPLATE_NAME * self, int size);

/* Changes the size of the dynamic array. Newsize must be >= 1. */
TEMPLATE_NAME * TEMPLATE_FUNC(size_)(TEMPLATE_NAME * self, int newsize);

/* Allocates a new array with size elements. */
TEMPLATE_NAME * TEMPLATE_FUNC(new)(int size);

/* Sets the amount of elements of the the array, but ony if
* amount bigger than the bigger than the current size.
* Returns NULL if the array was not grown, otherwise returns self.
*/
TEMPLATE_NAME * TEMPLATE_FUNC(grow)(TEMPLATE_NAME * self, int amount);

/* Returns a pointer to the index-th element of the array.
Does no bounds checking! */
TEMPLATE_T * TEMPLATE_FUNC(getptr_unsafe)(TEMPLATE_NAME * self, int index);

/* Returns the index-th element of the array. Does no bounds checking! */
TEMPLATE_T TEMPLATE_FUNC(getraw_unsafe)(TEMPLATE_NAME * self, int index);

/* Puts the element value at the index'th position 
* in the array. Does no bounds checking. Returns self;
*/
TEMPLATE_NAME * TEMPLATE_FUNC(putraw_unsafe)
(TEMPLATE_NAME * self, int index, TEMPLATE_T value);

/* Returns the index-th element of the array.
Does bounds checking and returns zero if out of bounds */
TEMPLATE_T TEMPLATE_FUNC(getraw)(TEMPLATE_NAME * self, 
                                 size_t index, TEMPLATE_T zero);

/* Returns the index-th element of the array in get.
Does bounds checking and returns FALSE if out of bounds or 
if get is not set. Returns TRUE and stores the result in get if all is OK. */
int TEMPLATE_FUNC(get)(TEMPLATE_NAME * self, 
                       int index, TEMPLATE_T * get);

/* Returns a pointer to the index-th element of the array.
Does bounds checking and return NULL if out of bounds */
TEMPLATE_T * TEMPLATE_FUNC(getptr)(TEMPLATE_NAME * self, int index);

/* Copies the TEMPLATE_FUNC(elementsize)(self) of bytes from the data 
* in value to the location pointed to by index.
* Does bounds checking and return NULL if out of bounds.
* Returns self if all was OK.
*/
TEMPLATE_NAME * TEMPLATE_FUNC(put)(TEMPLATE_NAME * self, 
  int index, TEMPLATE_T value);

/* Stores contents of a pointer at the index of the array.
* Does bounds checking.
*/
TEMPLATE_NAME * TEMPLATE_FUNC(putptr)(TEMPLATE_NAME * self, int index, 
                              TEMPLATE_T * ptr);






#else 
/* Implementation */

struct TEMPLATE_STRUCT {
  TEMPLATE_T * data;
  int size;
};


/* Gets the array's size. Returns 0 if self is NULL. */
int TEMPLATE_FUNC(size)(TEMPLATE_NAME * self) {
  if(!self) return 0;
  return self->size;
}

/* Frees the contents of an array. Has the same effect as emptying the array. 
Does not call a destructor on any elements contained! */
TEMPLATE_NAME * TEMPLATE_FUNC(done)(TEMPLATE_NAME * self) {
  if(!self) return NULL;
  TEMPLATE_FREE(self->data);
  self->data = NULL;
  self->size = 0;
  return self;
}

/* Frees an array. Returns NULL. */
TEMPLATE_NAME * TEMPLATE_FUNC(free)(TEMPLATE_NAME * self) {
  TEMPLATE_FUNC(done)(self);
  TEMPLATE_FREE(self);
  return NULL;
}

/* Allocates a new unusable array.  */
TEMPLATE_NAME * TEMPLATE_FUNC(alloc)() {
  return TEMPLATE_ALLOC(sizeof(TEMPLATE_NAME));
}


/* Initializes an empty array with size elements. */
TEMPLATE_NAME * TEMPLATE_FUNC(init)(TEMPLATE_NAME * self, int size) {
  if(!self)    return NULL;
  if(size < 0) return NULL;
  self->size = size;
  self->data = TEMPLATE_ALLOC(sizeof(TEMPLATE_T) * size);
  return self;
}

/* Changes the size of the dynamic array. Newsize must be >= 1. */
TEMPLATE_NAME * TEMPLATE_FUNC(size_)(TEMPLATE_NAME * self, int newsize) {
  TEMPLATE_T * newd = NULL;
  if(!self) return NULL;
  int index, stop;
  // Don't allow a newsize of 0, since that will make realloc call
  // free on self->data, which we don't want.
  if(newsize < 1) return NULL;
  newd = TEMPLATE_ALLOC(newsize * sizeof(TEMPLATE_T));
  if(!newd) return NULL;
  // copy data if needed
  if(self->data) { 
    stop  = self->size < newsize ? self->size : newsize;
    for(index = 0; index < self->size; index ++) {
      newd[index] = self->data[index];
    }
  }
  // if we get here alloc was successful, so it should be safe to reassign
  // self->data
  self->data = newd;
  self->size = newsize;
  return self;
}

/* Allocates a new array with size elements. Size should be greater than 0 */
TEMPLATE_NAME * TEMPLATE_FUNC(new)(int size) {
  TEMPLATE_NAME * res = TEMPLATE_FUNC(alloc)();
  if(!TEMPLATE_FUNC(init)(res, size)) {
      return TEMPLATE_FUNC(free)(res);
  }
  return res;
}


/* Sets the amount of elements of the the array, but ony if
* amount bigger than the bigger than the current size.
* Returns NULL if the array was not grown, otherwise returns self.
*/
TEMPLATE_NAME * TEMPLATE_FUNC(grow)(TEMPLATE_NAME * self, int amount) {
  int mysize = TEMPLATE_FUNC(size)(self);
  if (mysize >= amount) return NULL;
  return TEMPLATE_FUNC(size_)(self, amount);
}

/* Returns true if the index is out of range, false if it's OK to use 
with this TARRAY. */
int TEMPLATE_FUNC(outofrange)(TEMPLATE_NAME * self, int index) {
  if(!self) return TRUE;
  if (index < 0) return TRUE;
  return index >= TEMPLATE_FUNC(size)(self);
}

/* Returns a pointer to the index-th element of the array.
Does no bounds checking! */
TEMPLATE_T * TEMPLATE_FUNC(getptr_unsafe)(TEMPLATE_NAME * self, int index) {
  return self->data + index;
}

/* Returns the index-th element of the array. Does no bounds checking! */
TEMPLATE_T TEMPLATE_FUNC(getraw_unsafe)(TEMPLATE_NAME * self, int index) {
  return self->data[index];
}


/* Puts the element value at the index'th position 
* in the array. Does no bounds checking. Returns self;
*/
TEMPLATE_NAME * TEMPLATE_FUNC(putraw_unsafe)
(TEMPLATE_NAME * self, int index, TEMPLATE_T value) {
  self->data[index] = value;
  return self;
}

/* Returns the index-th element of the array.
Does bounds checking and returns zero if out of bounds */
TEMPLATE_T TEMPLATE_FUNC(getraw)(TEMPLATE_NAME * self, 
                                 size_t index, TEMPLATE_T zero) {
  // Bounds check
  if(TEMPLATE_FUNC(outofrange)(self, index)) { return zero; }
  return TEMPLATE_FUNC(getraw_unsafe)(self, index);
}

/* Returns the index-th element of the array in get.
Does bounds checking and returns FALSE if out of bounds or 
if get is not set. Returns TRUE and stores the result in get if all is OK*/
int TEMPLATE_FUNC(get)(TEMPLATE_NAME * self, 
                       int index, TEMPLATE_T * get) {
  // Bounds check
  if(!get) { return FALSE; }
  if(TEMPLATE_FUNC(outofrange)(self, index)) { return FALSE; }
  (*get) = TEMPLATE_FUNC(getraw_unsafe)(self, index);
  return TRUE;
}


/* Returns a pointer to the index-th element of the array.
Does bounds checking and return NULL if out of bounds */
TEMPLATE_T * TEMPLATE_FUNC(getptr)(TEMPLATE_NAME * self, int index) {
  // Bounds check
  if(TEMPLATE_FUNC(outofrange)(self, index)) { return NULL; }
  return TEMPLATE_FUNC(getptr_unsafe)(self, index);
}

/* Copies the TEMPLATE_FUNC(elementsize)(self) of bytes from the data 
* in value to the location pointed to by index.
* Does bounds checking and return NULL if out of bounds.
* Returns self if all was OK, NULL if not.
*/
TEMPLATE_NAME * TEMPLATE_FUNC(put)(TEMPLATE_NAME * self, 
  int index, TEMPLATE_T value) {
  // Bounds check
  if(TEMPLATE_FUNC(outofrange)(self, index)) { return NULL; }
  return TEMPLATE_FUNC(putraw_unsafe)(self, index, value);
}

/* Stores contents of a pointer at the index of the array.
* Does bounds checking.
*/
TEMPLATE_NAME * TEMPLATE_FUNC(putptr)(TEMPLATE_NAME * self, int index, 
                              TEMPLATE_T * ptr) {
  if(!ptr) return NULL; 
  return TEMPLATE_FUNC(put)(self, index, (*ptr));
  // use &ptr because we want to put the contents of the pointer,
  // not the pointer itself. 
}

#ifdef TEMPLATE_COMPARATOR

/* Applies quicksort to the array using the given comparator. */
TEMPLATE_NAME * TEMPLATE_FUNC(qsort)(TEMPLATE_NAME * self, 
                                     TEMPLATE_COMPARATOR * compare) {
  void * base; int nmemb; size_t size;
  if(!self) return NULL;
  base  = self->data;
  nmemb = self->size;
  size  = sizeof(TEMPLATE_T);
  qsort(base, nmemb, size, compare);
  return self;
}

#endif

#endif

#endif

#ifdef TEMPLATE_ZERO
#undef TEMPLATE_ZERO
#endif

#ifdef TEMPLATE_COMPARATOR
#undef TEMPLATE_COMPARATOR
#endif

/* Finally clean up by undefining all defined macros. **/
#include <tend.h>

#ifdef TEMPLATE_IMPLEMENT
#undef TEMPLATE_IMPLEMENT
#endif
/* We need this because of a nasty CPP bug in older (4.4.3) gcc versions: 
 * it seems the undefs in tend.h are not applied repeatedly. gcc 4.7.2 is ok. */




