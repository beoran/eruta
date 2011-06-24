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

/** Frees the contents of a self. */
SI_API SiBlock * siblock_done(SiBlock * self);

/** Initializes a new empty Block with elements of size elsz. */
SI_API SiBlock * siblock_initempty(SiBlock * self, size_t elsz);

/** Initializes a new Block with size elements of size elsz each. */
SI_API SiBlock * siblock_init(SiBlock * self, size_t room, size_t elsz);

/** Allocates a new Block with size elements of size elsz each. */
SI_API SiBlock * siblock_new(size_t size, size_t elsz);
 
 /** Allocates a new empty Block with size elements of size elsz each. */
SI_API SiBlock * siblock_newempty(size_t elsz);

/** Duplicates the self. */
SI_API SiBlock * siblock_dup(SiBlock * self);

/** Gets the  self's size. */
SI_API size_t siblock_size(SiBlock * self);

/** Gets the block's element size.  Returns 0 if self is NULL. */
SI_API size_t siblock_elementsize(SiBlock * self);

/** Gets the block's croom ( capacity).  Returns 0 if self is NULL. */
SI_API size_t siblock_room(SiBlock * self);

/** Resizes the self. Data is preserved. */
SI_API SiBlock * siblock_size_(SiBlock * self, size_t size);

/** Checks if the index is ok. */
SI_API int siblock_index_ok(SiBlock * self, size_t index);

SI_API int siblock_intindex_ok(SiBlock * self, int index);
 
/** Returns the index-th element of the self. */
SI_API void * siblock_get(SiBlock * self, size_t index);

/** Copies elsz of the the data in ptr to the index-th element of the self. */
SI_API void * siblock_set(SiBlock * self, size_t index, void * ptr);

/** Returns a new self that contains a slice of the elements of self. */
SI_API void * siblock_slice(SiBlock * self, size_t start, size_t amount);

/** Adds a new item to the self, at the last available index. */
SI_API void * siblock_add(SiBlock * self, void * ptr);

void * siblock_setptr(SiBlock * self, size_t index, void * ptr);

#ifdef SIBLOCK_EXTRA
/** Adds a new item to the self, at the last available index. */
SI_API void * siblock_addcopy(SiBlock * self, void * ptr);

/** Adds a new item to the self, at the last available index. */
SI_API void * siblock_addptr(SiBlock * self, void * ptr);

/** Returns the index-th element of the self. */
SI_API void * siblock_getcopy(SiBlock * self, size_t index);

/** Returns the index-th element of the self. */
SI_API void * siblock_getptr(SiBlock * self, size_t index);

/** Copies elsz of the the data in ptr to the index-th element of the self. */
SI_API void * siblock_set(SiBlock * self, size_t index, void * ptr);

/** Copies elsz of the the data in ptr to the index-th element of the self. */
SI_API void * siblock_setptr(SiBlock * self, size_t index, void * ptr);

#endif

#endif


