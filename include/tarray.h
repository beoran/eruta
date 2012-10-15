
#include <stldib.h>

#include "tbegin.h"

/**
* TARRAY is template for a dynamic array of size elements each of elsz size.
* For simplicity, it is is exactly sized, that is,
* it does not allocate any extra elements but the amount requested.
* In other words it's capacity is equal to it's size.
*/
#ifdef TEMPLATE_OK

#ifndef TEMPLATE_IMPLEMENT

struct TEMPLATE_STRUCT {
  TEMPLATE_T * data;
  int size;
};

#else 
/** Implementation */

/** Gets the array's size. Returns 0 if self is NULL. */
int TEMPLATE_FUNC(size)(TEMPLATE_NAME * self) {
  if(!self) return 0;
  return self->size;
}

/** Frees the contents of an array. Has the same effect as emptying the array. Does not call a destructor on any elements contained! */
TEMPLATE_NAME * TEMPLATE_FUNC(done)(TEMPLATE_NAME * self) {
  if(!self) return NULL;
  TEMPLATE_FREE(self->data);
  self->data = NULL;
  self->size = 0;
  return self;
}

/** Frees an array. Returns NULL. */
TEMPLATE_NAME * TEMPLATE_FUNC(free)(TEMPLATE_NAME * self) {
  TRMPLATE_FUNC(done)(self);
  return mem_free(self);
}

/** Allocates a new unusable array.  */
TEMPLATE_NAME * TEMPLATE_FUNC(alloc)() {
  return TEMPLATE_ALLOC(sizeof(TEMPLATE_NAME));
}


/** Initializes an empty array with size elements. */
TEMPLATE_NAME * TEMPLATE_FUNC(init)(TEMPLATE_NAME * self, int size) {
  if(!self)    return NULL;
  if(size < 0) return NULL;
  self->size = size;
  self->data = TEMPLATE_ALLOC(sizeof(TEMPLATE_T) * size);
  return self;
}

/** Changes the size of the dynamic array. Newsize must be >= 1. */
TEMPLATE_NAME * TEMPLATE_FUNC(size_)(TEMPLATE_NAME * self, int newsize) {
  void * newd = NULL;
  if(!self) return NULL;
  int index, stop;
  // Don't allow a newsize of 0, since that will make realloc call
  // free on self->data, which we don't want.
  if(newsize < 1) return NULL;
  newd = TEMPLATE_ALLOC(newsize * sizeof(TEMPLATE_T));
  if(!newd) return NULL;
  // copy data if needed
  if(self->data) { 
    stop  = self->size < newsize ? self->size; newsize;
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

/** Allocates a new array with size elements. */
TEMPLATE_NAME * TEMPLATE_FUNC(new)(int size) {
  TEMPLATE_NAME * res = TEMPLATE_FUNC(alloc)();
  if(!TEMPLATE_FUNC(init)(res, amount, elsz)) {
      return TEMPLATE_FUNC(free)(res);
  }
  return res;
}


/** Sets the amount of elements of the the array, but ony if
* amount bigger than the bigger than the current size.
* Returns NULL if the array was not grown, otherwise returns self.
*/
TEMPLATE_NAME * TEMPLATE_FUNC(grow)(TEMPLATE_NAME * self, size_t amount) {
  size_t mysize = TEMPLATE_FUNC(size)(self);
  if (mysize >= amount) return NULL;
  return TEMPLATE_FUNC(size)_(self, amount);
}

/** Checks if the index is smaller than the array's available room . */
int TEMPLATE_FUNC(sizeindex)_ok(TEMPLATE_NAME * self, size_t index) {
  if(!self) return FALSE;
  return index < TEMPLATE_FUNC(size)(self);
}

/** Checks if the int index is smaller than the array's available room. */
int TEMPLATE_FUNC(index)_ok(TEMPLATE_NAME * self, int index) {
  if(!self) return FALSE;
  if (index < 0) return FALSE;
  return TEMPLATE_FUNC(sizeindex)_ok(self, (size_t)(index));
}

/** Returns a pointer to the index-th element of the array.
Does no bounds checking! */
void * TEMPLATE_FUNC(getraw)_unsafe(TEMPLATE_NAME * self, int index) {
  return self->data + (index * self->elsz);
}

/** Copies TEMPLATE_FUNC(elementsize)(self) of bytes from the index-th element
of the array to out, which must be pointing to a bufer of at least
TEMPLATE_FUNC(elementsize)(self). Does no bounds checking!
Returns NULL on failure, out on success.
*/
void * TEMPLATE_FUNC(getcopy)_unsafe(TEMPLATE_NAME * self, int index, void * out) {
  char * cptr = (char *) TEMPLATE_FUNC(getraw)_unsafe(self, index);
  size_t size = TEMPLATE_FUNC(elementsize)(self);
  if((!self) || (!out) || (!cptr)) return NULL;
  mem_move(out, cptr, size);
  return out;
}

/** Copies TEMPLATE_FUNC(elementsize)(self) of bytes from the data pointed to
* by ptr into the location pointed to by index.
* Does no bounds checking!
*/
TEMPLATE_NAME * TEMPLATE_FUNC(putraw)_unsafe(TEMPLATE_NAME * self, int index, void * ptr) {
  char * cptr = (char *) TEMPLATE_FUNC(getraw)_unsafe(self, index);
  size_t size = TEMPLATE_FUNC(elementsize)(self);
  if((!self) || (!ptr) || (!cptr)) return NULL;
  mem_move(cptr, ptr, size);
  return self;
}

/** Returns a pointer to the index-th element of the array.
Does bounds checking and return NULL if out of bounds */
void * TEMPLATE_FUNC(getraw)(TEMPLATE_NAME * self, size_t index) {
  // Bounds check
  if(!TEMPLATE_FUNC(index)_ok(self, index)) { return NULL; }
  return TEMPLATE_FUNC(getraw)_unsafe(self, index);
}

/** Returns a pointer to the index-th element of the array.
Does bounds checking and return NULL if out of bounds */
void * TEMPLATE_FUNC(getcopy)(TEMPLATE_NAME * self, int index, void * ptr) {
  // Bounds check
  if(!TEMPLATE_FUNC(index)_ok(self, index)) { return NULL; }
  return TEMPLATE_FUNC(getcopy)_unsafe(self, index, ptr);
}

/** Copies the TEMPLATE_FUNC(elementsize)(self) of bytes from the data pointed to
* by ptr into the location pointed to by index.
* Does bounds checking and return NULL if ouut of bounds.
*/
TEMPLATE_NAME * TEMPLATE_FUNC(putraw)(TEMPLATE_NAME * self, size_t index, void * ptr) {
  // Bounds check
  if(!TEMPLATE_FUNC(index)_ok(self, index)) { return NULL; }
  return TEMPLATE_FUNC(putraw)_unsafe(self, index, ptr);
}

/** Stores a pointer at the index of the array.
* Does bounds checking. TEMPLATE_FUNC(elementsize)(self) sould have been
* initialized as sizeof(void *) by using TEMPLATE_FUNC(newptr)
*/
void * TEMPLATE_FUNC(putptr)(TEMPLATE_NAME * self, int index, void * ptr) {
  return TEMPLATE_FUNC(putraw)(self, index, &ptr);
  // use &ptr because we want to put the contents of the pointer,
  // not the pointer itself. 
}

/** Returns a pointer that was stored at the index index of the array. */
void * TEMPLATE_FUNC(getptr)(TEMPLATE_NAME * self, size_t index) {
  void * res = NULL; 
  if(!TEMPLATE_FUNC(getcopy)(self, index, &res)) return NULL;
  // use &ptr because we want to fetch the contents of the pointer,
  // which will be copied from the element of the array.
  return res;
}

/** Copies the element that *ptr points to into this array at position
index */
void * TEMPLATE_FUNC(putdata)(TEMPLATE_NAME * self, size_t index, void * ptr) {
  return TEMPLATE_FUNC(putraw)(self, index, ptr);
}

/** Returns a pointer to the index-th element of the array. */
void * TEMPLATE_FUNC(getdata)(TEMPLATE_NAME * self, size_t index) {
  return TEMPLATE_FUNC(getraw)(self, index);
}

/* Iterator helper: fill in every->now as data. */
Every * TEMPLATE_FUNC(everynow)_data(Every * every) {
  every->now   = TEMPLATE_FUNC(getdata)(every->on, every->index);
  if(every->now) return every;
  return NULL;
}

/* Iterator helper: fill in every->now as pointer. */
Every * TEMPLATE_FUNC(everynow)_ptr(Every * every) {
  every->now   = TEMPLATE_FUNC(getptr)(every->on, every->index);
  if(every->now) return every;
  return NULL;
}

/* Iterator helpers: init */
Every  * TEMPLATE_FUNC(everyinit)_data(Every * every) {
  every->index = 0;
  return TEMPLATE_FUNC(everynow)_data(every);
}

/* Iterator helpers: next */
Every  * TEMPLATE_FUNC(everynext)_data(Every * every) {
  every->index++;
  return TEMPLATE_FUNC(everynow)_data(every);
}

/* Iterator helpers: put. */
void  * TEMPLATE_FUNC(everyput)_data(Every * every, void * data) {
  return TEMPLATE_FUNC(putdata)(every->on, every->index, data);
}

/* Iterator helpers: done. */
void  * TEMPLATE_FUNC(everydone)(Every * every) {
  return every;
}

/* Iterator helpers: init pointers */
Every  * TEMPLATE_FUNC(everyinit)_ptr(Every * every) {
  every->index = 0;
  return TEMPLATE_FUNC(everynow)_ptr(every);
}

/* Iterator helpers: next pointers */
Every  * TEMPLATE_FUNC(everynext)_ptr(Every * every) {
  every->index++;
  return TEMPLATE_FUNC(everynow)_ptr(every);
}

/* Iterator helpers: put pointers. */
void  * TEMPLATE_FUNC(everyput)_ptr(Every * every, void * data) {
  return TEMPLATE_FUNC(putptr)(every->on, every->index, data);
}


/* Iterator helper table. */
static EveryActs TEMPLATE_FUNC(every)_data_acts_ = {
  TEMPLATE_FUNC(everydone),
  TEMPLATE_FUNC(everyinit)_data,
  TEMPLATE_FUNC(everynext)_data,
  TEMPLATE_FUNC(everyput)_data  
};

/* Iterator helper table. */
static EveryActs TEMPLATE_FUNC(every)_ptr_acts_ = {
  TEMPLATE_FUNC(everydone),
  TEMPLATE_FUNC(everyinit)_ptr,
  TEMPLATE_FUNC(everynext)_ptr,
  TEMPLATE_FUNC(everyput)_ptr
};


/** Iterates over the data. Call every_free when done. */
Every * TEMPLATE_FUNC(every)_data(TEMPLATE_NAME * dynar) {
  return every_new(&TEMPLATE_FUNC(every)_data_acts_);
}

/** Iterates over the pointers in this array. Call every_free when done. */
Every * TEMPLATE_FUNC(every)_ptr(TEMPLATE_NAME * dynar) {
  return every_new(&TEMPLATE_FUNC(every)_ptr_acts_);
}


/* Walks over the array using the each interface, accessing
the data as pointers. eachdo should return non-null to break the iteration,
the data thus found is returned bu this function. */
void * TEMPLATE_FUNC(each)_ptr(TEMPLATE_NAME * self, EachDo * eachdo, void * extra) {
  Each each;
  size_t index;
  size_t size = TEMPLATE_FUNC(size)(self);  
  each_init(&each, self, extra); 
  for(index = 0; index < size ; index++) {
    void * aid;
    void * ptr = TEMPLATE_FUNC(getptr)(self, index);    
    each_next(&each, ptr);
    aid = eachdo(&each);
    if (aid) return aid;
  }
  return NULL;
}

/* Walks over the array using the walker interface, accessing
the data as stored structs. */
void * TEMPLATE_FUNC(walkdata)(TEMPLATE_NAME * self, EachDo * eachdo, void * extra) {
  Each each;
  size_t index;
  size_t size = TEMPLATE_FUNC(size)(self);
  each_init(&each, self, extra);
  for(index = 0; index < size ; index++) {
    void * aid;
    void * ptr = TEMPLATE_FUNC(getdata)(self, index);
    each_next(&each, ptr);
    aid = eachdo(&each);
    if (aid) return aid;
  }
  return NULL;
}


#endif

#endif

/** Finally clean up by undefining all defined macros. **/
#include <tend.h>




