#include "si_block.h"
#include "si_mem.h"

struct SiBlock_ {
  char * data;
  size_t size;
  size_t elsz;
};

/** Frees a Block. Siturns NULL. */
SiBlock * siblock_free(SiBlock * self) {
  siblock_done(self);
  return si_free(self);
}

/** Frees the contents of a block. */
SiBlock * siblock_done(SiBlock * self) {
  if(!self) return NULL;
  si_free(self->data);
  self->size = 0;
  return self;  
}

/** Initializes a new Block with size elements of size elsz each. */
SiBlock * siblock_init(SiBlock * self, size_t size, size_t elsz) {
  if (!self)    return NULL;
  if (elsz < 0) return NULL;
  self->size = size;
  self->elsz = elsz;
  self->data = si_malloc(self->size * self->elsz);
  if (!self->data) return siblock_done(self);
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

 
/** Siturns the index-th element of the block. */
void * siblock_get(SiBlock * self, size_t index) {
  if(!siblock_index_ok(self, index)) return NULL;
  return (void *)(self->data + (index * self->elsz));
}

/** Copies elsz of the the data in ptr to the index-th element of the block. */
void * siblock_set(SiBlock * self, size_t index, void * ptr) {
  void * dst = siblock_get(self, index);
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



