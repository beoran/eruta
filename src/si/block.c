#include "si_block.h"
#include "si_mem.h"

#define SI_BLOCK_ROOM 16

struct SiBlock_ {
  char * data;
  size_t size;
  size_t elsz;
  size_t room;
};

/** Frees a Block. Returns NULL. */
SiBlock * siblock_free(SiBlock * self) {
  siblock_done(self);
  return si_free(self);
}

/** Frees the contents of a block. Has the same effect as emptying the block. */
SiBlock * siblock_done(SiBlock * self) {
  if(!self) return NULL;
  si_free(self->data);
  self->size = 0;
  self->room = 0;
  return self;  
}

/** Initializes a new Block with size elements of size elsz each. */
SiBlock * siblock_init(SiBlock * self, size_t size, size_t elsz) {
  if (!self)    return NULL;
  if (elsz < 0) return NULL;
  self->room = SI_BLOCK_ROOM;
  if (self->room < size) {
    self->room = SI_BLOCK_ROOM + size;
  }
  self->size = size;
  self->elsz = elsz;
  if((self->room * self->elsz) < 1) {
    self->data = NULL;
  } else {
    self->data = si_malloc(self->room * self->elsz);
    if (!self->data) return siblock_done(self);
  }
  return self;
}

/** Changes the room available in the SiBlock. */
SiBlock * siblock_room_(SiBlock * self, size_t room) {
  void * aid = NULL;
  if(!self) return NULL;
  aid        = si_realloc(self->data, room); 
  if(!aid) return NULL;
  self->room = room;
  self->data = aid;
  return self;
}

/** Grows the SiBlock if needed. */
SiBlock * siblock_room_grow(SiBlock * self, size_t newroom) {
  if(!self) return NULL;
  if (newroom >= self->room) return siblock_room_(self, newroom + SI_BLOCK_ROOM);
  return self;
}


/** Allocates a new empty Block.  */
SiBlock * siblock_alloc() {
  return si_malloc(sizeof(SiBlock));
}  

/** Allocates a new Block with size elements of size elsz each. */
SiBlock * siblock_new(size_t size, size_t elsz) {
  SiBlock * res = siblock_alloc();
  if(!siblock_init(res, size, elsz)) {
    siblock_free(res);
    return NULL;
  }
  return res;
} 

/** Allocates a new empty Block for elements of size elsz each. */
SiBlock * siblock_newempty(size_t elsz) {
  return siblock_new(0, elsz);
}


/** Gets the  block's size. */
size_t siblock_size(SiBlock * self) {
  return self->size;
}

/** Gets the block's element size. */
size_t siblock_elementsize(SiBlock * self) {
  return self->elsz;
}

/** Sisizes the block. Data is preserved. */
size_t siblock_size_(SiBlock * self, size_t size);

/** Checks if the index is ok. */
int siblock_index_ok(SiBlock * self, size_t index) {
  return index < self->size;
}
 

/** Copies size elements of data from ptr into the block starting at index. */
SiBlock * siblock_xcopyptr(SiBlock * self, size_t selfi, 
                        void * ptr, size_t ptrs, size_t ptri, size_t amount) {
  if((!self) || (!self->data)) return NULL;
  si_smemcpy(self->data, self->size, selfi, ptr, ptrs, ptri, amount * self->elsz);
  return self;
}

/** Copies elemetrs from one block to the next, starting at the given indexes.
Element size must be identical. */
SiBlock * siblock_xcopy(SiBlock * self , size_t selfi,
                        SiBlock * block, size_t blocki, 
                        size_t amount) {
  if((!self)  || (!self->data))   return NULL;
  if((!block) || (!block->data))  return NULL;
  if(block->elsz != self->elsz)   return NULL;
  si_smemcpy(self->data , self->size , selfi  * self->elsz, 
             block->data, block->size, blocki * block->elsz, 
             amount * self->elsz);
  return self;
}

 
/** Returns the index-th element of the block. */
void * siblock_get(SiBlock * self, size_t index) {
  if(!siblock_index_ok(self, index)) return NULL;
  return (void *)(self->data + (index * self->elsz));
}

/** Copies elsz of the the data in ptr to the index-th element of the block.
* This may grow the block if the index is out of range. 
*/
void * siblock_set(SiBlock * self, size_t index, void * ptr) {
  void * dst;
  if(!siblock_room_grow(self, index)) return NULL; // make space if needed.
  dst = siblock_get(self, index);
  if(!dst) return NULL;
  si_memcpy(dst, ptr, self->elsz);
  return dst;
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
SI_API void * siblock_add(SiBlock * self, void * ptr) {
  size_t last = siblock_size(self);
  return siblock_set(self, last, ptr); 
}




