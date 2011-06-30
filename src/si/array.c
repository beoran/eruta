#include "si_array.h"
#include "si_mem.h"

#define SI_ARRAY_ROOM 16

struct SiArray_; 
typedef struct SiArray_ SiArray;

struct SiArray_ {
  SiMem * mem;
  size_t size;
  size_t elsz;
};

/** Gets the  array's size. Returns 0 if self is NULL. */
size_t siarray_size(SiArray * self) {
    if(!self) return 0;
    return self->size;
}

/** Gets the array's element size.  Returns 0 if self is NULL. */
size_t siarray_elementsize(SiArray * self) {
  if(!self) return 0;
  return self->elsz;
}

/** Gets the array's croom ( capacity).  Returns 0 if self is NULL. */
size_t siarray_room(SiArray * self) {
  if(!self) return 0;
  return simem_room(self->mem);
}

/** Frees the contents of an array. Has the same effect as emptying the array.
*/
SiArray * siarray_done(SiArray * self) {
  if(!self)  return NULL;
  simem_free(self->mem);
  self->mem  = NULL;
  self->size = 0;
  return self;
}

/** Frees an array. Returns NULL. */
SiArray * siarray_free(SiArray * self) {
  siarray_done(self);
  return si_free(self);
}

/** Allocates a new empty array.  */
SiArray * siarray_alloc() {
  return si_malloc(sizeof(SiArray));
}  


/** Initializes an empty array with elements of size elsz each. */
SiArray * siarray_initempty(SiArray * self, size_t elsz) {
  if(!self)    return NULL;
  if(elsz < 0) return NULL;
  self->size = 0;
  self->elsz = elsz;
  return self;
}


/** Allocates a new empty array for elements of size elsz each. */
SiArray * siarray_newempty(size_t elsz) {
  SiArray * res = siarray_alloc();
  if(!siarray_initempty(res, elsz)) { 
    return siarray_free(res);
  }
  return res;
}


/** Changes the room available in the SiArray. */
SiArray * siarray_room_(SiArray* self, size_t newroom) {
  if(!self) return NULL; 
  if(!self->mem) { 
    self->mem  = simem_new(newroom);
    if(!self->mem) return NULL;
    return self;
  } 
  simem_realloc(self->mem, newroom);   
  return self;
}

/** Grows the SiArray if needed. */
SiArray * siarray_grow(SiArray * self, size_t newroom) {
  if(!self) return NULL;
  if (newroom > siarray_room(self)) { 
    return siarray_room_(self, newroom);
  }
  return self;
}

/** Initializes a new array with a capacity of room elements of size 
elsz each. The initial size of the array will be 0.*/
SiArray * siarray_init(SiArray * self, size_t room, size_t elsz) {
  SiArray * aid = siarray_initempty(self, elsz);
  if(!aid) return NULL;
  if(!siarray_grow(self, room)) return NULL;
  return self;
}

/** Allocates a new array with size elements of size elsz each. */
SiArray * siarray_new(size_t size, size_t elsz) {
  SiArray * res = siarray_alloc();    
  if(!siarray_init(res, size, elsz)) {
      return siarray_free(res);
  }
  return res;
} 

/** Sets the size of the array. This will grow the room also if size <= room.
* Returns NULL if the resizing failed, otherwise returns self. 
* This cannot shrink the size, though.
*/
SiArray * siarray_size_(SiArray * self, size_t size) {
  // need at least one more room than size, 
  // but generously give even more
  SiArray * aid = siarray_grow(self, size + SI_ARRAY_ROOM);
  if(!aid) return NULL;
  self->size = size;
  return self;
}

/** Checks if the index is smaller than the array's available room. */
int siarray_index_ok(SiArray * self, size_t index) {
  if(!self) return FALSE;
  return index < self->size;
}

/** Checks if the int index is smaller than the array's available room. */
int siarray_intindex_ok(SiArray * self, int index) {
  if(!self) return FALSE;
  if (index < 0) return FALSE;
  return (size_t)(index) < siarray_room(self);
}




#ifdef _COMMENT  

/** Returns a pointer to the index-th element of the array. 
Performs no range checking */
void * siarray_getraw(SiArray * self, size_t index) { 
  return (void *)(self->data + (index * self->elsz));
}
 
/** Returns the index-th element of the array. 
Performs a range check and returns NULL if out of range. */
void * siarray_getdata(SiArray * self, size_t index) {
  if(!siarray_index_ok(self, index)) return NULL;
  return siarray_getraw(self, index);
}


/** Copies elsz of the the data in ptr to the index-th element of the array.
* This may grow the array if the index is out of range. 
*/
void * siarray_setdata(SiArray * self, size_t index, void * ptr) {
  void * dst;  
  if(!siarray_size_(self, index+1)) return NULL; 
  // Set the size to index + 1 if needed.
  dst = siarray_getraw(self, index);
  if(!dst) return NULL;
  si_memmove(dst, ptr, self->elsz);
  if (index >= self->size) { 
    // Also grow size if needed.
    self->size = index + 1;
  } 
  return dst;
}

/** Stores a pointer at index index. elementize should be sizeof(void *) */
void * siarray_setptr(SiArray * self, size_t index, void * ptr) {
  return siarray_setdata(self, index, &ptr);
}

/** Retrieves a pointer at index index. elementize should be sizeof(void *) */
void * siarray_getptr(SiArray * self, size_t index) {
  void ** aid = siarray_getdata(self, index);
  if(!aid) return NULL;
  return *(aid);
}

/** Adds a new pointer to self, at the last available index. */
void * siarray_addptr(SiArray * self, void * ptr) {
  return siarray_adddata(self, &ptr);
}


/** Siturns a new block that contains a slice of the elements of self. */
void * siarray_slice(SiArray * self, size_t start, size_t amount) {
  SiArray * res = NULL; 
  if(!self) return NULL;
  res = siarray_new(amount, self->elsz);
  if(!res) return NULL;
  return siarray_xcopy(res , 0, self, start, amount);
}

/** Duplicates the array. */
SiArray * siarray_dup(SiArray * self) {
  return siarray_slice(self, 0, siarray_size(self));
}

/** Adds a new item to the array, at the last available index. */
void * siarray_adddata(SiArray * self, void * ptr) {
  size_t last = siarray_size(self);
  void * ok   = siarray_setdata(self, last, ptr);
  if(!ok) return NULL;
  return self;
}

/** Walks over the array using the walker interface, accessing 
the data as pointers. */
void * siarray_walkptr(SiArray * self, SiWalker * walker, void * extra) {
  size_t index;
  size_t size = siarray_size(self);  
  for(index = 0; index < size ; index++) {
    void * ptr = siarray_getptr(self, index); 
    void * aid = walker(ptr, &index, self, extra);
    if (aid) return aid;
  }
  return NULL;
}

/** Walks over the array using the walker interface, accessing 
the data as stored structs. */
void * siarray_walkdata(SiArray * self, SiWalker * walker, void * extra) {
  size_t index;
  size_t size = siarray_size(self);  
  for(index = 0; index < size ; index++) {
    void * ptr = siarray_getdata(self, index); 
    void * aid = walker(ptr, &index, self, extra);
    if (aid) return aid;
  }
  return NULL;
}


#endif


