#ifndef SI_BLOCK_H
#define SI_BLOCK_H

#include <stdlib.h>
#include "si_macro.h"
#include "si_walker.h"

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
SI_API void * siblock_getdata(SiBlock * self, size_t index);

/** Copies elsz of the the data in ptr to the index-th element of the self. */
SI_API void * siblock_setdata(SiBlock * self, size_t index, void * ptr);

/** Returns a new self that contains a slice of the elements of self. */
SI_API void * siblock_slice(SiBlock * self, size_t start, size_t amount);

/** Adds a new item to the SiBlock self, at the last available index. */
SI_API void * siblock_adddata(SiBlock * self, void * ptr);

/** Copies the pointer ptr itself to the index-th element of self. */
SI_API void * siblock_setptr(SiBlock * self, size_t index, void * ptr);

/** Returns the pointer stored at index with siblock_setptr*/
SI_API void * siblock_getptr(SiBlock * self, size_t index);

/** Adds a new pointer to self, at the last available index. */
SI_API void * siblock_addptr(SiBlock * self, void * ptr);


/** Walks over the Block using the walker interface, accessing 
the data as pointers. */
void * siblock_walkptr(SiBlock * self, SiWalker * walker, void * extra);

/** Walks over the Block using the walker interface, accessing 
the data as stored structs. */
void * siblock_walkdata(SiBlock * self, SiWalker * walker, void * extra);


#endif


