#ifndef SI_BLOCK_H
#define SI_BLOCK_H

#include <stdlib.h>
#include "si_macro.h"

/** A Block is a simply array-like structure that owns and frees it's own 
cells, but /not/ any data (e.g, pointers) copied in the cells. */
struct SiBlock_;
typedef struct SiBlock_ SiBlock;

/** Frees a Block. Returns NULL. */
SI_API SiBlock * siblock_free(SiBlock * self);

/** frees the contents of a block. */
SI_API SiBlock * siblock_done(SiBlock * self);

/** Initializes a new Block with size elements of size elsz each. */
SI_API SiBlock * siblock_init(SiBlock * self, size_t size, size_t elsz);

/** Allocates a new Block with size elements of size elsz each. */
SI_API SiBlock * siblock_new(size_t size, size_t elsz); 

/** Duplicates the block. */
SI_API SiBlock * siblock_dup(SiBlock * block);

/** Gets the  block's size. */
SI_API size_t siblock_size(SiBlock * block);

/** Resizes the block. Data is preserved. */
SI_API size_t siblock_size_(SiBlock * block, size_t size);

/** Checks if the index is ok. */
SI_API int siblock_index_ok(SiBlock * block, size_t index);
 
/** Returns the index-th element of the block. */
SI_API void * siblock_get(SiBlock * block, size_t index);

/** Copies elsz of the the data in ptr to the index-th element of the block. */
SI_API void * siblock_set(SiBlock * block, size_t index, void * ptr);

/** Returns a new block that contains a slice of the elements of self. */
SI_API void * siblock_slice(SiBlock * block, size_t start, size_t amount);






#endif


