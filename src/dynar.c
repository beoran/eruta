#include "dynar.h"
#include "mem.h"

#define ARRAY_ROOM 16

/**
* Dynar is a DYNamic ARray of size elements each of elsz size.
* For simplicity, dynarr is exactly sized, that is,
* it does not allocate any extra elements but the amount requested.
* In other words it's capacity is equal to it's size.
*/
struct Dynar_ {
  char * data;
  // use a char * pointer siince that makes pointer arithmetic easier
  size_t size;
  size_t elsz;
};

/** Gets the array's size. Returns 0 if self is NULL. */
size_t dynar_size(Dynar * self) {
  if(!self) return 0;
  return self->size;
}

/** Gets the amount of elements in the vectore. Returns 0 if self is NULL. */
size_t dynar_amount(Dynar * self) {
  if(!self) return 0;
  return self->size;
}

/** Gets the array's element size.  Returns 0 if self is NULL. */
size_t dynar_elementsize(Dynar * self) {
  if(!self) return 0;
  return self->elsz;
}

/** Frees the contents of an array. Has the same effect as emptying the array. Does not call a desctructor on any elements contained! */
Dynar * dynar_done(Dynar * self) {
  if(!self) return NULL;
  mem_free(self->data);
  self->data = NULL;
  self->size = 0;
  return self;
}

/** Frees an array. Returns NULL. */
Dynar * dynar_free(Dynar * self) {
  dynar_done(self);
  return mem_free(self);
}

/** Allocates a new empty array.  */
Dynar * dynar_alloc() {
  return STRUCT_ALLOC(Dynar);
}  


/** Initializes an empty array with 0 elements of size elsz each. */
Dynar * dynar_initempty(Dynar * self, size_t elsz) {
  if(!self)    return NULL;
  if(elsz < 0) return NULL;
  self->size = 0;
  self->elsz = elsz;
  return self;
}

/** Allocates a new empty array for elements of size elsz each. */
Dynar * dynar_newempty(size_t elsz) {
  Dynar * res = dynar_alloc();
  if(!dynar_initempty(res, elsz)) {
    return dynar_free(res);
  }
  return res;
}

/** Changes the size of the dynamic array. Newsize must be >= 1. */
Dynar * dynar_size_(Dynar* self, size_t newsize) {
  size_t elsz = dynar_elementsize(self);
  void * newd = NULL;
  if(!self) return NULL;
  // don't allow a newsize of 0, since that will make realloc call
  // free on self->data, which we don't want.
  if(!newsize < 1) return NULL;
  newd = mem_realloc(self->data, newsize * elsz);
  if(!newd) return NULL;
  // if we get here realloc was successful, so it should be safe to reassign
  // self->data
  self->data = newd;
  self->size = newsize;  
  return self;
}

/** Initializes a new array with a amount elements of size
elsz each. */
Dynar * dynar_init(Dynar * self, size_t amount, size_t elsz) {
  Dynar * aid = dynar_initempty(self, elsz);
  if(!aid) return NULL;
  if(!dynar_size_(self, amount)) return NULL;
  return self;
}

/** Initializes a new array with a capacity to store amount void* pointers.*/
Dynar * dynar_initptr(Dynar * self, size_t amount) {
  return dynar_init(self, amount, sizeof(void *));
}

/** Allocates a new array with amount elements of size elsz each. */
Dynar * dynar_new(size_t amount, size_t elsz) {
  Dynar * res = dynar_alloc();
  if(!dynar_init(res, amount, elsz)) {
      return dynar_free(res);
  }
  return res;
}

/** Allocates a new array that will be able to contain amount void * pointers.*/
Dynar * dynar_newptr(size_t amount) {
  return dynar_new(amount, sizeof(void *));
}
 

/** Sets the amount of elements of the the array, but ony if
* amount bigger than the bigger than the current size.
* Returns NULL if the array was not grown, otherwise returns self.
*/
Dynar * dynar_grow(Dynar * self, size_t amount) {
  size_t mysize = dynar_size(self);
  if (mysize >= amount) return NULL;
  return dynar_size_(self, amount);
}

/** Checks if the index is smaller than the array's available room . */
int dynar_sizeindex_ok(Dynar * self, size_t index) {
  if(!self) return FALSE;
  return index < dynar_size(self);
}

/** Checks if the int index is smaller than the array's available room. */
int dynar_index_ok(Dynar * self, int index) {
  if(!self) return FALSE;
  if (index < 0) return FALSE;
  return dynar_sizeindex_ok(self, (size_t)(index));
}

/** Returns a pointer to the index-th element of the array.
Does no bounds checking! */
void * dynar_getraw_unsafe(Dynar * self, int index) {
  return self->data + (index * self->elsz);
}

/** Copies dynar_elementsize(self) of bytes from the index-th element
of the array to out, which must be pointing to a bufer of at least
dynar_elementsize(self). Does no bounds checking!
Returns NULL on failure, out on success.
*/
void * dynar_getcopy_unsafe(Dynar * self, int index, void * out) {
  char * cptr = (char *) dynar_getraw_unsafe(self, index);
  size_t size = dynar_elementsize(self);
  if((!self) || (!out) || (!cptr)) return NULL;
  mem_move(out, cptr, size);
  return out;
}

/** Copies dynar_elementsize(self) of bytes from the data pointed to
* by ptr into the location pointed to by index.
* Does no bounds checking!
*/
Dynar * dynar_putraw_unsafe(Dynar * self, int index, void * ptr) {
  char * cptr = (char *) dynar_getraw_unsafe(self, index);
  size_t size = dynar_elementsize(self);
  if((!self) || (!ptr) || (!cptr)) return NULL;
  mem_move(cptr, ptr, size);
  return self;
}

/** Returns a pointer to the index-th element of the array.
Does bounds checking and return NULL if ouut of bounds */
void * dynar_getraw(Dynar * self, size_t index) {
  // Bounds check
  if(!dynar_index_ok(self, index)) { return NULL; }
  return dynar_getraw_unsafe(self, index);
}

/** Returns a pointer to the index-th element of the array.
Does bounds checking and return NULL if ouut of bounds */
void * dynar_getcopy(Dynar * self, int index, void * ptr) {
  // Bounds check
  if(!dynar_index_ok(self, index)) { return NULL; }
  return dynar_getcopy_unsafe(self, index, ptr);
}

/** Copies the dynar_elementsize(self) of bytes from the data pointed to
* by ptr into the location pointed to by index.
* Does bounds checking and return NULL if ouut of bounds.
*/
Dynar * dynar_putraw(Dynar * self, size_t index, void * ptr) {
  // Bounds check
  if(!dynar_index_ok(self, index)) { return NULL; }
  return dynar_putraw_unsafe(self, index, ptr);
}

/** Stores a pointer at the index of the array.
* Does bounds checking. dynar_elementsize(self) sould have been
* initialized as sizeof(void *) by using dynar_newptr
*/
void * dynar_putptr(Dynar * self, int index, void * ptr) {
  return dynar_putraw(self, index, &ptr);
  // use &ptr because we want to put the contents of the pointer,
  // not the pointer itself. 
}

/** Returns a pointer that was stored at the index index of the array. */
void * dynar_getptr(Dynar * self, size_t index) {
  void * res = NULL; 
  if(!dynar_getcopy(self, index, &res)) return NULL;
  // use &ptr because we want to fetch the contents of the pointer,
  // which will be copied from the element of the array.
  return res;
}

/** Copies the element that *ptr points to into this array at position
index */
void * dynar_putdata(Dynar * self, size_t index, void * ptr) {
  return dynar_putraw(self, index, ptr);
}

/** Returns a pointer to the index-th element of the array. */
void * dynar_getdata(Dynar * self, size_t index) {
  return dynar_getraw(self, index);
}

/* Iterator helper: fill in every->now as data. */
Every * dynar_everynow_data(Every * every) {
  every->now   = dynar_getdata(every->on, every->index);
  if(every->now) return every;
  return NULL;
}

/* Iterator helper: fill in every->now as pointer. */
Every * dynar_everynow_ptr(Every * every) {
  every->now   = dynar_getptr(every->on, every->index);
  if(every->now) return every;
  return NULL;
}

/* Iterator helpers: init */
Every  * dynar_everyinit_data(Every * every) {
  every->index = 0;
  return dynar_everynow_data(every);
}

/* Iterator helpers: next */
Every  * dynar_everynext_data(Every * every) {
  every->index++;
  return dynar_everynow_data(every);
}

/* Iterator helpers: put. */
void  * dynar_everyput_data(Every * every, void * data) {
  return dynar_putdata(every->on, every->index, data);
}

/* Iterator helpers: done. */
void  * dynar_everydone(Every * every) {
  return every;
}

/* Iterator helpers: init pointers */
Every  * dynar_everyinit_ptr(Every * every) {
  every->index = 0;
  return dynar_everynow_ptr(every);
}

/* Iterator helpers: next pointers */
Every  * dynar_everynext_ptr(Every * every) {
  every->index++;
  return dynar_everynow_ptr(every);
}

/* Iterator helpers: put pointers. */
void  * dynar_everyput_ptr(Every * every, void * data) {
  return dynar_putptr(every->on, every->index, data);
}


/* Iterator helper table. */
static EveryActs dynar_every_data_acts_ = {
  dynar_everydone,
  dynar_everyinit_data,
  dynar_everynext_data,
  dynar_everyput_data  
};

/* Iterator helper table. */
static EveryActs dynar_every_ptr_acts_ = {
  dynar_everydone,
  dynar_everyinit_ptr,
  dynar_everynext_ptr,
  dynar_everyput_ptr
};


/** Iterates over the data. Call every_free when done. */
Every * dynar_every_data(Dynar * dynar) {
  return every_new(&dynar_every_data_acts_);
}

/** Iterates over the pointers in this array. Call every_free when done. */
Every * dynar_every_ptr(Dynar * dynar) {
  return every_new(&dynar_every_ptr_acts_);
}



#ifdef COMMENT_

/* Walks over the array using the walker interface, accessing
the data as pointers. */
void * dynar_walkptr(Dynar * self, Walker * walker, void * extra) {
  size_t index;
  size_t size = dynar_size(self);
  for(index = 0; index < size ; index++) {
    void * ptr = dynar_getptr(self, index);
    void * aid = walker(ptr, &index, self, extra);
    if (aid) return aid;
  }
  return NULL;
}

/* Walks over the array using the walker interface, accessing
the data as stored structs. */
void * dynar_walkdata(Dynar * self, Walker * walker, void * extra) {
  size_t index;
  size_t size = dynar_size(self);
  for(index = 0; index < size ; index++) {
    void * ptr = dynar_getdata(self, index);
    void * aid = walker(ptr, &index, self, extra);
    if (aid) return aid;
  }
  return NULL;
}

#endif





