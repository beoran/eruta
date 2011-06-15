#include "re/block.h"
#include "re/mem.h"

struct ReBlock_ {
  char * data;
  size_t size;
  size_t elsz;
};

/** Frees a Block. Returns NULL. */
ReBlock * reblock_free(ReBlock * self) {
  reblock_done(self);
  return re_free(self);
}

/** Frees the contents of a block. */
ReBlock * reblock_done(ReBlock * self) {
  if(!self) return NULL;
  re_free(self->data);
  self->size = 0;
  return self;  
}

/** Initializes a new Block with size elements of size elsz each. */
ReBlock * reblock_init(ReBlock * self, size_t size, size_t elsz) {
  if (!self)    return NULL;
  if (elsz < 0) return NULL;
  self->size = size;
  self->elsz = elsz;
  self->data = re_malloc(self->size * self->elsz);
  if (!self->data) return reblock_done(self);
  return self;  
}

/** Allocates a new empty Block.  */
ReBlock * reblock_alloc() {
  return re_malloc(sizeof(ReBlock));
}  

/** Allocates a new Block with size elements of size elsz each. */
ReBlock * reblock_new(size_t size, size_t elsz) {
  ReBlock * res = reblock_alloc();
  if(!reblock_init(res, size, elsz)) {
    reblock_free(res);
    return NULL;
  }
  return res;
} 


/** Gets the  block's size. */
size_t reblock_size(ReBlock * self) {
  return self->size;
}

/** Gets the block's element size. */
size_t reblock_elementsize(ReBlock * self) {
  return self->elsz;
}

/** Resizes the block. Data is preserved. */
size_t reblock_size_(ReBlock * self, size_t size);

/** Checks if the index is ok. */
int reblock_index_ok(ReBlock * self, size_t index) {
  return index < self->size;
}
 

/** Copies size elements of data from ptr into the block starting at index. */
ReBlock * reblock_xcopyptr(ReBlock * self, size_t selfi, 
                        void * ptr, size_t ptrs, size_t ptri, size_t amount) {
  if((!self) || (!self->data)) return NULL;
  re_smemcpy(self->data, self->size, selfi, ptr, ptrs, ptri, amount * self->elsz);
  return self;
}

/** Copies elemetrs from one block to the next, starting at the given indexes.
Element size must be identical. */
ReBlock * reblock_xcopy(ReBlock * self , size_t selfi,
                        ReBlock * block, size_t blocki, 
                        size_t amount) {
  if((!self)  || (!self->data))   return NULL;
  if((!block) || (!block->data))  return NULL;
  if(block->elsz != self->elsz)   return NULL;
  re_smemcpy(self->data , self->size , selfi  * self->elsz, 
             block->data, block->size, blocki * block->elsz, 
             amount * self->elsz);
  return self;
}

 
/** Returns the index-th element of the block. */
void * reblock_get(ReBlock * self, size_t index) {
  if(!reblock_index_ok(self, index)) return NULL;
  return (void *)(self->data + (index * self->elsz));
}

/** Copies elsz of the the data in ptr to the index-th element of the block. */
void * reblock_set(ReBlock * self, size_t index, void * ptr) {
  void * dst = reblock_get(self, index);
  if(!dst) return NULL;
  re_memcpy(dst, ptr, self->elsz);
  return dst;
}

/** Returns a new block that contains a slice of the elements of self. */
void * reblock_slice(ReBlock * self, size_t start, size_t amount) {
  ReBlock * res = NULL; 
  if(!self) return NULL;
  res = reblock_new(amount, self->elsz);
  if(!res) return NULL;
  return reblock_xcopy(res , 0, self, start, amount); 
}

/** Duplicates the block. */
ReBlock * reblock_dup(ReBlock * self) {
  return reblock_slice(self, 0, reblock_size(self));
}



