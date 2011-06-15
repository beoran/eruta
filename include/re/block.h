#ifndef _RE_BOCK_H_
#define _RE_BOCK_H_

#include <stdlib.h>

/** A Block is a simply array-like structure that owns and frees it's own 
cells, but /not/ any data (e.g, pointers) copied in the cells. */
struct ReBlock_;
typedef struct ReBlock_ ReBlock;

/** Frees a Block. Returns NULL. */
ReBlock * reblock_free(ReBlock * self);

/** frees the contents of a block. */
ReBlock * reblock_done(ReBlock * self);

/** Initializes a new Block with size elements of size elsz each. */
ReBlock * reblock_init(ReBlock * self, size_t size, size_t elsz);

/** Allocates a new Block with size elements of size elsz each. */
ReBlock * reblock_new(size_t size, size_t elsz); 

/** Duplicates the block. */
ReBlock * reblock_dup(ReBlock * block);

/** Gets the  block's size. */
size_t reblock_size(ReBlock * block);

/** Resizes the block. Data is preserved. */
size_t reblock_size_(ReBlock * block, size_t size);

/** Checks if the index is ok. */
int reblock_index_ok(ReBlock * block, size_t index);
 
/** Returns the index-th element of the block. */
void * reblock_get(ReBlock * block, size_t index);

/** Copies elsz of the the data in ptr to the index-th element of the block. */
void * reblock_set(ReBlock * block, size_t index, void * ptr);

/** Returns a new block that contains a slice of the elements of self. */
void * reblock_slice(ReBlock * block, size_t start, size_t amount);






#endif


