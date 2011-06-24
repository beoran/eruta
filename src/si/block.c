#include "si_block.h"
#include "si_mem.h"

#define SI_BLOCK_ROOM 16

struct SiBlock_ {
  char * data;
  size_t size;
  size_t elsz;
  size_t room;
};

/** Gets the  block's size. Returns 0 if self is NULL. */
size_t siblock_size(SiBlock * self) {
    if(!self) return 0;
    return self->size;
}

/** Gets the block's element size.  Returns 0 if self is NULL. */
size_t siblock_elementsize(SiBlock * self) {
  if(!self) return 0;
  return self->elsz;
}

/** Gets the block's croom ( capacity).  Returns 0 if self is NULL. */
size_t siblock_room(SiBlock * self) {
  if(!self) return 0;
  return self->room;
}

/** Frees the contents of a block. Has the same effect as emptying the block. */
SiBlock * siblock_done(SiBlock * self) {
  if(!self)  return NULL;
  si_free(self->data);
  self->data = NULL;
  self->size = 0;
  self->room = 0;
  return self;
}

/** Frees a Block. Returns NULL. */
SiBlock * siblock_free(SiBlock * self) {
  siblock_done(self);
  return si_free(self);
}

/** Allocates a new empty Block.  */
SiBlock * siblock_alloc() {
  return si_malloc(sizeof(SiBlock));
}  


/** Initializes an empty Block with elements of size elsz each. */
SiBlock * siblock_initempty(SiBlock * self, size_t elsz) {
  if(!self)    return NULL;
  if(elsz < 0) return NULL;
  self->data = NULL;
  self->size = 0;
  self->room = 0;
  self->elsz = elsz;
  return self;
}


/** Allocates a new empty Block for elements of size elsz each. */
SiBlock * siblock_newempty(size_t elsz) {
  SiBlock * res = siblock_alloc();
  if(!siblock_initempty(res, elsz)) { 
    return siblock_free(res);
  }
  return res;
}


/** Changes the room available in the SiBlock. */
SiBlock * siblock_room_(SiBlock* self, size_t newroom) {
  void * aid = NULL;
  if(!self) return NULL;
  aid        = si_realloc(self->data, newroom * self->elsz); 
  if(!aid) return NULL;
  self->room = newroom;
  self->data = aid;
  // Also shrink size.
  if(self->size > newroom) { self->size = newroom; }
  return self;
}

/** Grows the SiBlock if needed. */
SiBlock * siblock_grow(SiBlock * self, size_t newroom) {
  if(!self) return NULL;
  if (newroom > self->room) { 
    return siblock_room_(self, newroom);
  }
  return self;
}

/** Initializes a new Block with a capacity of room elements of size 
elsz each. The initial size of the block will be 0.*/
SiBlock * siblock_init(SiBlock * self, size_t room, size_t elsz) {
  SiBlock * aid = siblock_initempty(self, elsz);
  if(!aid) return NULL;
  if(!siblock_grow(self, room)) return NULL;
  return self;
}

/** Allocates a new Block with size elements of size elsz each. */
SiBlock * siblock_new(size_t size, size_t elsz) {
  SiBlock * res = siblock_alloc();    
  if(!siblock_init(res, size, elsz)) {
      return siblock_free(res);
  }
  return res;
} 

/** Sets the size of the block. This will grow the room also if size <= room.
* Returns NULL if the resizing failed, otherwise returns self. 
* This cannot shrink the size, though.
*/
SiBlock * siblock_size_(SiBlock * self, size_t size) {
  // need at least one more room than size, 
  // but generously give even more
  SiBlock * aid = siblock_grow(self, size + SI_BLOCK_ROOM);
  if(!aid) return NULL;
  self->size = size;
  return self;
}

/** Checks if the index is smaller than the block's size. */
int siblock_index_ok(SiBlock * self, size_t index) {
  if(!self) return FALSE;
  return index < self->size;
}

/** Checks if the int index is smaller than the block's size. */
int siblock_intindex_ok(SiBlock * self, int index) {
  if(!self) return FALSE;
  if (index < 0) return FALSE;
  return (size_t)(index) < self->room;
}
 

/** Copies size elements of data from ptr into the block starting at index. */
SiBlock * siblock_xcopyptr(SiBlock * self, size_t selfi, 
                      void * ptr, size_t ptrs, size_t ptri, size_t amount) {
    if((!self) || (!self->data)) 
        return NULL;
    si_smemcpy(self->data, self->size, selfi, ptr, ptrs, ptri, amount *
    self->elsz);
    return self;
}

/** Copies elemets from one block to the next, starting at the given indexes.
Element size must be identical. */
SiBlock * siblock_xcopy(SiBlock * self , size_t selfi,
                        SiBlock * block, size_t blocki, 
                        size_t amount) {
      if((!self)  || (!self->data))         return NULL;
      if((!block) || (!block->data))        return NULL;
      if(block->elsz != self->elsz)         return NULL;
      si_smemcpy(self->data , self->size , selfi  * self->elsz, 
             block->data, block->size, blocki * block->elsz, 
             amount * self->elsz);
      return self;
}

/** Returns a pointer to the index-th element of the block. 
Performs no range checking */
void * siblock_getraw(SiBlock * self, size_t index) { 
  return (void *)(self->data + (index * self->elsz));
}
 
/** Returns the index-th element of the block. 
Performs a range check and returns NULL if out of range. */
void * siblock_getdata(SiBlock * self, size_t index) {
  if(!siblock_index_ok(self, index)) return NULL;
  return siblock_getraw(self, index);
}


/** Copies elsz of the the data in ptr to the index-th element of the block.
* This may grow the block if the index is out of range. 
*/
void * siblock_setdata(SiBlock * self, size_t index, void * ptr) {
  void * dst;  
  if(!siblock_size_(self, index+1)) return NULL; 
  // Set the size to index + 1 if needed.
  dst = siblock_getraw(self, index);
  if(!dst) return NULL;
  si_memmove(dst, ptr, self->elsz);
  if (index >= self->size) { 
    // Also grow size if needed.
    self->size = index + 1;
  } 
  return dst;
}

/** Stores a pointer at index index. elementize should be sizeof(void *) */
void * siblock_setptr(SiBlock * self, size_t index, void * ptr) {
  return siblock_setdata(self, index, &ptr);
}

/** Retrieves a pointer at index index. elementize should be sizeof(void *) */
void * siblock_getptr(SiBlock * self, size_t index) {
  void ** aid = siblock_getdata(self, index);
  if(!aid) return NULL;
  return *(aid);
}

/** Adds a new pointer to self, at the last available index. */
void * siblock_addptr(SiBlock * self, void * ptr) {
  return siblock_adddata(self, &ptr);
}


/** Siturns a new block that contains a slice of the elements of self. */
void * siblock_slice(SiBlock * self, size_t start, size_t amount) {
  SiBlock * res = NULL; 
  if(!self) return NULL;
  res = siblock_new(amount, self->elsz);
  if(!res) return NULL;
  return siblock_xcopy(res , 0, self, start, amount);
}

/** Duplicates the block. */
SiBlock * siblock_dup(SiBlock * self) {
  return siblock_slice(self, 0, siblock_size(self));
}

/** Adds a new item to the block, at the last available index. */
void * siblock_adddata(SiBlock * self, void * ptr) {
  size_t last = siblock_size(self);
  void * ok   = siblock_setdata(self, last, ptr);
  if(!ok) return NULL;
  return self;
}

/** Walks over the Block using the walker interface, accessing 
the data as pointers. */
void * siblock_walkptr(SiBlock * self, SiWalker * walker, void * extra) {
  size_t index;
  size_t size = siblock_size(self);  
  for(index = 0; index < size ; index++) {
    void * ptr = siblock_getptr(self, index); 
    void * aid = walker(ptr, &index, self, extra);
    if (aid) return aid;
  }
  return NULL;
}

/** Walks over the Block using the walker interface, accessing 
the data as stored structs. */
void * siblock_walkdata(SiBlock * self, SiWalker * walker, void * extra) {
  size_t index;
  size_t size = siblock_size(self);  
  for(index = 0; index < size ; index++) {
    void * ptr = siblock_getdata(self, index); 
    void * aid = walker(ptr, &index, self, extra);
    if (aid) return aid;
  }
  return NULL;
}





