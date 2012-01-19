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
  void * data;
  size_t size;
  size_t elsz;
};

/** Gets the array's size. Returns 0 if self is NULL. */
size_t dynar_size(Dynar * self) {
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


/** Initializes an empty array with elements of size elsz each. */
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

/** Changes the room available in the Dynar. */
Dynar * dynar_room_(Dynar* self, size_t newroom) {
  size_t elsz = dynar_elementsize(self);
  if(!self) return NULL; 
  if(!self->mem) { 
    self->mem  = simem_newelement(newroom, elsz);
    if(!self->mem) return NULL;
    return self;
  } 
  if(!simem_reallocelement(self->mem, newroom, elsz)) return NULL;
  return self;
} 

/** Grows the Dynar if needed so it will be able to contain amount elements.*/
Dynar * dynar_grow(Dynar * self, size_t amount) {
  if(!self) return NULL;
  if (amount > dynar_room(self)) {
    return dynar_room_(self, amount);
  }
  return self;
}

/** Initializes a new array with a capacity of amount elements of size 
elsz each. The initial size of the array will be 0.*/
Dynar * dynar_init(Dynar * self, size_t amount, size_t elsz) {
  Dynar * aid = dynar_initempty(self, elsz);
  if(!aid) return NULL;
  amount = ( amount < 1  ? SI_ARRAY_ROOM : amount );
  if(!dynar_grow(self, amount)) return NULL;
  return self;
}

/** Initializes a new array with a capacity to store amount void* pointers. 
The initial size of the array will be 0.*/
Dynar * dynar_initptr(Dynar * self, size_t amount) {
  return dynar_init(self, amount, sizeof(void *));
}

/** Allocates a new array with size elements of size elsz each. */
Dynar * dynar_new(size_t size, size_t elsz) {
  Dynar * res = dynar_alloc();
  if(!dynar_init(res, size, elsz)) {
      return dynar_free(res);
  }
  return res;
}

/** Allocates a new array that will be able to contain amount void * pointers.*/
Dynar * dynar_newptr(size_t amount) {
  return dynar_new(amount, sizeof(void *));
}
 

/** Sets the size of the array. This fails if the array's room is less than 
* or size, and return null. Otherwise sets size and returns self 
* if there enough room for the given size. 
* Returns NULL if there is not enough room, otherwise returns self.
*/
Dynar * dynar_size_(Dynar * self, size_t size) {
  size_t room = dynar_room(self);
  if (size > room) return NULL;
  self->size = size;
  return self;
}

/** Sets the size of the array, but ony if size is bigger than the current size.
* Returns NULL if there is not enough room, otherwise returns self.
*/
Dynar * dynar_biggersize(Dynar * self, size_t size) {
  size_t mysize = dynar_size(self);
  if (mysize >= size) return NULL;  
  return dynar_size_(self, size);
}

/** Increments the size of the array by one. Does not grow the array. */
Dynar * dynar_sizeup(Dynar * self) {
  size_t mysize = dynar_size(self);
  return dynar_size_(self, mysize + 1);
}

/** Decrements the size of the array by one. Does not shrink the array. */
Dynar * dynar_sizedown(Dynar * self) {
  size_t mysize = dynar_size(self);
  return dynar_size_(self, mysize - 1);
}


/** Checks if the index is smaller than the array's available room . */
int dynar_index_ok(Dynar * self, size_t index) {
  if(!self) return FALSE;
  return index < dynar_room(self);
}

/** Checks if the int index is smaller than the array's available room. */
int dynar_intindex_ok(Dynar * self, int index) {
  if(!self) return FALSE;
  if (index < 0) return FALSE;
  return (size_t)(index) < dynar_room(self);
}

/** Returns a pointer to the index-th element of the array. */
void * dynar_getraw(Dynar * self, size_t index) {
  return simem_getelement(self->mem, index, dynar_elementsize(self));
}

/** Stores a pointer that was stored at the index index of the array. */
void * dynar_putptr(Dynar * self, size_t index, void * ptr) {
  return simem_putptr(self->mem, index, ptr);
}

/** Returns a pointer that was stored at the index index of the array. */
void * dynar_getptr(Dynar * self, size_t index) {
  return simem_getptr(self->mem, index);
}

/** Copies the element that *ptr points to into index */
void * dynar_putdata(Dynar * self, size_t index, void * ptr) {
  return simem_putelement(self->mem, index, ptr, dynar_elementsize(self));
}

/** Returns a pointer to the index-th element of the array. */
void * dynar_getdata(Dynar * self, size_t index) {
  return simem_getelement(self->mem, index, dynar_elementsize(self));
}

/** Returns nonzero if the array is "full", that isn, it's size is 
equal to the available room.  Zero if it isn't. */
int dynar_full_p(Dynar *self) {
  return dynar_size(self) == dynar_room(self);
}

#ifdef _COMMENT  

/** Returns a pointer to the index-th element of the array. 
Performs no range checking */
void * dynar_getraw(Dynar * self, size_t index) {
  return (void *)(self->data + (index * self->elsz));
}
 
/** Returns the index-th element of the array. 
Performs a range check and returns NULL if out of range. */
void * dynar_getdata(Dynar * self, size_t index) {
  if(!dynar_index_ok(self, index)) return NULL;
  return dynar_getraw(self, index);
}


/** Copies elsz of the the data in ptr to the index-th element of the array.
* This may grow the array if the index is out of range. 
*/
void * dynar_setdata(Dynar * self, size_t index, void * ptr) {
  void * dst;  
  if(!dynar_size_(self, index+1)) return NULL;
  // Set the size to index + 1 if needed.
  dst = dynar_getraw(self, index);
  if(!dst) return NULL;
  si_memmove(dst, ptr, self->elsz);
  if (index >= self->size) { 
    // Also grow size if needed.
    self->size = index + 1;
  } 
  return dst;
}

/** Stores a pointer at index index. elementize should be sizeof(void *) */
void * dynar_setptr(Dynar * self, size_t index, void * ptr) {
  return dynar_setdata(self, index, &ptr);
}

/** Retrieves a pointer at index index. elementize should be sizeof(void *) */
void * dynar_getptr(Dynar * self, size_t index) {
  void ** aid = dynar_getdata(self, index);
  if(!aid) return NULL;
  return *(aid);
}

/** Adds a new pointer to self, at the last available index. */
void * dynar_addptr(Dynar * self, void * ptr) {
  return dynar_adddata(self, &ptr);
}


/** Siturns a new block that contains a slice of the elements of self. */
void * dynar_slice(Dynar * self, size_t start, size_t amount) {
  Dynar * res = NULL;
  if(!self) return NULL;
  res = dynar_new(amount, self->elsz);
  if(!res) return NULL;
  return dynar_xcopy(res , 0, self, start, amount);
}

/** Duplicates the array. */
Dynar * dynar_dup(Dynar * self) {
  return dynar_slice(self, 0, dynar_size(self));
}

/** Adds a new item to the array, at the last available index. */
void * dynar_adddata(Dynar * self, void * ptr) {
  size_t last = dynar_size(self);
  void * ok   = dynar_setdata(self, last, ptr);
  if(!ok) return NULL;
  return self;
}

/** Walks over the array using the walker interface, accessing 
the data as pointers. */
void * dynar_walkptr(Dynar * self, SiWalker * walker, void * extra) {
  size_t index;
  size_t size = dynar_size(self);
  for(index = 0; index < size ; index++) {
    void * ptr = dynar_getptr(self, index);
    void * aid = walker(ptr, &index, self, extra);
    if (aid) return aid;
  }
  return NULL;
}

/** Walks over the array using the walker interface, accessing 
the data as stored structs. */
void * dynar_walkdata(Dynar * self, SiWalker * walker, void * extra) {
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


