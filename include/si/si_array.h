#ifndef SI_ARRAY_H
#define SI_ARRAY_H
#include "si.h"


struct SiArray_; 
typedef struct SiArray_ SiArray;

/** Gets the  array's size. Returns 0 if self is NULL. */
size_t siarray_size(SiArray * self);

/** Gets the array's element size.  Returns 0 if self is NULL. */
size_t siarray_elementsize(SiArray * self);

/** Gets the array's room (capacity) in amount of elemnts. 
 Returns 0 if self is NULL. */
size_t siarray_room(SiArray * self);

/** Frees the contents of an array. Has the same effect as emptying the array.
*/
SiArray * siarray_done(SiArray * self);

/** Frees an array. Returns NULL. */
SiArray * siarray_free(SiArray * self);

/** Allocates a new empty array.  */
SiArray * siarray_alloc();

/** Initializes an empty array with elements of size elsz each. */
SiArray * siarray_initempty(SiArray * self, size_t elsz);


/** Allocates a new empty array for elements of size elsz each. */
SiArray * siarray_newempty(size_t elsz);

/** Changes the room available in the SiArray. */
SiArray * siarray_room_(SiArray* self, size_t newroom);

/** Grows the SiArray if needed so it will be able to contain amount elements.*/
SiArray * siarray_grow(SiArray * self, size_t amount);

/** Initializes a new array with a capacity of amount elements of size 
elsz each. The initial size of the array will be 0.*/
SiArray * siarray_init(SiArray * self, size_t amount, size_t elsz);

/** Initializes a new array with a capacity to store amount void* pointers. 
The initial size of the array will be 0.*/
SiArray * siarray_initptr(SiArray * self, size_t amount);

/** Allocates a new array with size elements of size elsz each. */
SiArray * siarray_new(size_t size, size_t elsz);

/** Allocates a new array that will be able to contain amount void * pointers.*/
SiArray * siarray_newptr(size_t amount);
 

/** Sets the size of the array. This will grow fail if there is not 
* enough room for the given size. 
* Returns NULL if there is not enough room, otherwise returns self.
*/
SiArray * siarray_size_(SiArray * self, size_t size);

/** Checks if the index is smaller than the array's available room . */
int siarray_index_ok(SiArray * self, size_t index);

/** Checks if the int index is smaller than the array's available room. */
int siarray_intindex_ok(SiArray * self, int index);

/** Returns a pointer to the index-th element of the array. */
void * siarray_getraw(SiArray * self, size_t index);

/** Returns a pointer that was stored at the index index of the array. */
void * siarray_getptr(SiArray * self, size_t index);

/** Copies the element that *ptr points to into index */
void * siarray_setdata(SiArray * self, size_t index, void * ptr);

/** Returns a pointer to the index-th element of the array. */
void * siarray_getdata(SiArray * self, size_t index);




#endif
