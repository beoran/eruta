#ifndef SI_MEM_H
#define SI_MEM_H

#include <stdlib.h>
#include "si_macro.h"


/** Allocate memory */
SI_API void * si_malloc(size_t size);

/** Free memory. */
SI_API void * si_free(void * ptr);
 
/** Copy memory. */ 
SI_API void * si_memcpy(void * dst, void * src, size_t size);

/** Copy overlapping memory. */ 
SI_API void * si_memmove(void * dst, void * src, size_t size);


/** Reallocate memory. */
SI_API void * si_realloc(void * ptr, size_t size);

/* Safely copies memory between src and dst. dsts and srcs are the sizes of 
src and dst, dsti and srci are the indexes at which to start copying, 
amount is the amount of data to copy.  */
SI_API void * si_smemcpy(void * dst, size_t dsts, size_t dsti, 
                  void * src, size_t srcs, size_t srci, 
                  size_t amount);

/** Allocate and copy memory. */
SI_API void * si_copyalloc(size_t size, void * src,  size_t tocopy);


/** Shorthand for si_malloc(sizof(STRUCTNAME)) */
#define SI_ALLOCATE(STRUCTNAME) si_malloc(sizeof(STRUCTNAME))

/** Shorthand for STRUCTNAME * FUNCNAME() { return si_malloc(sizof(STRUCTNAME)); } */
#define SI_DEFINE_ALLOCATE(STRUCTNAME, FUNCNAME) \
  STRUCTNAME * FUNCNAME() { return (STRUCTNAME *) si_malloc(sizof(STRUCTNAME));}

/** SiMem is a structure that models a block of allocated memory of a given
size. It allows to do pointer arithmetic in a safer way. */
struct SiMem_;
typedef struct SiMem_ SiMem;

/** The struct is exposed for inheritance, however, it's members should 
be considered private. Only use the functions available to manipulate
this struct. */
struct SiMem_ {
  char * data;
  size_t room;
};

/** Makes a memory block unusable. automatically called when using simem_free.*/
SiMem * simem_done(SiMem * self);

/** Frees simem and returns NULL. */
SiMem * simem_free(SiMem * self);

/* Initializes a SiMem block */
SiMem * simem_init(SiMem* self, size_t room); 

/** Initializes a new memory block. Room must be greater than 0. */
SiMem * simem_new(size_t room); 

/** Initializes a new memory block with amount elements the size of elementsize.
Amount must be greater than 0. */
SiMem * simem_newelement(size_t amount, size_t elementsize);

/** Nondestructive resize of the memory as per realloc. Room must be > 0. */
SiMem * simem_realloc(SiMem * self, size_t room);

/** Room available in the memory block. */
size_t simem_room(SiMem * self);

/** Returns true if ndex is in range for self->data, false if not.*/
int simem_indexok(SiMem * self, size_t index);

/** Gets a character from the given index. */
char simem_getc(SiMem * self, size_t index);

/** Puts a character at the given index. */
char simem_putc(SiMem * self, size_t index, char c);

/** compares the character at index with compare. */
int simem_cmpc(SiMem * self, size_t index, char compare);

/** Copies size bytes data from a void pointer to the SiMem, starting at index. 
Returns amount of bytes actually copied. */
int simem_putdata(SiMem * self, size_t index,  void * ptr, size_t size);

/** Copies size bytes data to a void pointer to the SiMem, starting at index. 
Returns amount of bytes actually copied. */
int simem_getdata(SiMem * self, size_t index,  void * ptr, size_t size);

/** Compares a data block with the data  ptr points to, starting at index,
with size size */
SiMem * simem_cmpdata(SiMem * self, size_t index, void * ptr, size_t size);

/** Copies data from SiMem src to Simem dst, starting at srci and dsti,
copying size bytes.  Returns amount of bytes actually copied.  */
int simem_copy(SiMem * dst, size_t dsti, SiMem * src, size_t srci, size_t size);

/** Copies all data from src to dst starting at indexes 0,0 */
int simem_copyall(SiMem *dst, SiMem *src);

/** Fills a data block with the character value, starting at index, with size
size */
SiMem * simem_fill(SiMem * self, char value, size_t index, size_t size);

/** Fills a data block completely with the character value*/
SiMem * simem_fillall(SiMem * self, char value);

/** Duplicates a data block. */
SiMem * simem_dup(SiMem * self);

/** returns the head pointer of the memory block. */
size_t simem_ptr(SiMem * self);

/** Stores elemsize of data at index * elsize. */
void * simem_putelement(SiMem * self, size_t index, void * ptr, size_t elsize);

/** Gets a pointer to the data at index * elsize. */
void * simem_getelement(SiMem * self, size_t index, size_t elsize);

/** Stores a pointer at index * sizeof(void *). */
void * simem_putptr(SiMem * self, size_t index, void * ptr);

/** retrieves a pointer from the memeory block self at 
index * sizeof(void *). */
void * simem_getptr(SiMem * self, size_t index);

/** Nondestructive resize of the memory as per realloc, so that the mem
will be able to contain amount elements of elementsize. Both must be > 0. */
SiMem * simem_reallocelement(SiMem * self, size_t amount, size_t elementsize);

/** Nondestructive resize of the memory as per realloc, so that the mem
will be able to contain amount void* pointers */
SiMem * simem_reallocptr(SiMem * self, size_t amount);

/** Returns the amount of elements of elementsize this mem block can contain.*/
size_t simem_roomelement(SiMem * self, size_t elementsize);

/** Returns the amount of void * pointers this mem block can contain.*/
size_t simem_roomptr(SiMem * self);



#endif
