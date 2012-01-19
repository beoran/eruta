#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "si_mem.h"


void * si_malloc(size_t size) {
  if (size < 1) return NULL;
  return calloc(size, 1);
}

void * si_free(void * ptr) {
 free(ptr);
 return NULL;
}


void * si_smemcpy(void * dst, size_t dsts, size_t dsti, 
                  void * src, size_t srcs, size_t srci, 
                  size_t amount) {
  size_t index = 0;
  if(!dst) return NULL;
  if(!src) return NULL;
  while((dsti < dsts) && (srci < srcs) && (index < amount)) {
    uint8_t c               = ((uint8_t *)src)[srci];
    ((uint8_t *)dst)[dsti]  = c;
    dsti++; srci++; index++; 
  } 
  return dst;
}

void * si_memcpy(void * dst, void * src, size_t size) {
  return si_smemcpy(dst, size, 0 , src , size, 0, size);  
}

/** Copy overlapping memory. */ 
void * si_memmove(void * dst, void * src, size_t size) {
  return memmove(dst, src, size);
}



void * si_realloc(void * ptr, size_t size) {
  return realloc(ptr, size);
}

/** Allocate and copy memory. */
void * si_copyalloc(size_t size, void * src,  size_t tocopy) {
  void * res = si_malloc(size);
  if(!res) return NULL;
  return si_memcpy(res,src,tocopy); 
}



SiMem * simem_done(SiMem * self) {
  si_free(self->data); 
  self->data = NULL;
  self->room = 0;
  return self;
}

SiMem * simem_free(SiMem * self) {
  simem_done(self);
  return si_free(self); 
}

SiMem * simem_init(SiMem* self, size_t room) {
  if(!self) return NULL;
  self->room = room;
  self->data = (char *) si_malloc(self->room);  
  if(self->data) return self;
  return NULL;
}

/** Initializes a new memory block. Room must be greater than 0. */
SiMem * simem_new(size_t room) {
  SiMem * self  = (SiMem *) si_malloc(sizeof(SiMem));
  if(!self) return NULL;
  if(simem_init(self, room)) return self; // if init ok return self. 
  return si_free(self); // if we get here init failed. 
}

/** Initializes a new memory block with amount elements the size of elementsize.
Amount must be greater than 0. */
SiMem * simem_newelement(size_t amount, size_t elementsize) {
  return simem_new(amount * elementsize);
}

/** Initializes a new memory block that can store amount void * pointers.
Amount must be greater than 0. */
SiMem * simem_newptr(size_t amount) {
  return simem_newelement(amount, sizeof(void *));
}


/** Nondestructive resize of the memory as per realloc. Room must be > 0. */
SiMem * simem_realloc(SiMem * self, size_t room) {
  char * aid = (char *) si_realloc(self->data, room);
  if (aid) {
    self->data = aid;
    self->room = room;
    return self; 
  }
  return NULL; 
  // if we get here, change nothing but return NULL to signal the
  // realloc error.
} 

/** Nondestructive resize of the memory as per realloc, so that the mem
will be able to contain amount and elementsize must be > 0. */
SiMem * simem_reallocelement(SiMem * self, size_t amount, size_t elementsize) {
  return simem_realloc(self, amount * elementsize); 
}

/** Nondestructive resize of the memory as per realloc, so that the mem
will be able to contain amount void* pointers */
SiMem * simem_reallocptr(SiMem * self, size_t amount) {
  return simem_reallocelement(self, amount, sizeof(void*)); 
}

/** Returns the amount of elements of elementsize this mem block can contain.
* elementsize must be greater than 0. Returns 0 on error.
*/
size_t simem_roomelement(SiMem * self, size_t elementsize) {
  if(!self)        return 0;
  if(!elementsize) return 0;
  return self->room / elementsize;
}

/** Returns the amount of void * pointers this mem block can contain.*/
size_t simem_roomptr(SiMem * self) {
  return simem_roomelement(self, sizeof(void *));
}


/** Room available in the memory block. */
size_t simem_room(SiMem * self) {
  return self->room;
}


/** Returns true if ndex is in range for self->data, false if not.*/
int simem_indexok(SiMem * self, size_t index) {
  if(!self) return FALSE;
  return (index < self->room);
}


/** Gets a character from the given index. Returns 0 if out of bounds. */
char simem_getc(SiMem * self, size_t index) {
  if(!simem_indexok(self, index)) return 0;
  return self->data[index];
}


/** Compares a character from the given index with ch. 
Returns -2 if out of bounds,  -1 if the char at location is less, 0 if 
equal 1 if it is more. */
int simem_cmpc(SiMem * self, size_t index, char compare) {
  char ch;
  if(!simem_indexok(self, index)) return -2;
  ch = self->data[index];
  if (ch == compare) return 0;
  if (ch < compare) return -1;
  return 1;
}


/** Puts a character at the given index. */
char simem_putc(SiMem * self, size_t index, char c) {
  if(!simem_indexok(self, index)) return 0;
  return self->data[index] = c;
}

/** Compares a data block with the data  ptr points to, starting at index,
with size size */
SiMem * simem_cmpdata(SiMem * self, size_t index, void * ptr, size_t size) {
  char * cptr = (char *) ptr;  
  size_t room, pdex = 0;  
  room = simem_room(self);
  for(pdex = 0; (index < room) && (pdex < size); index++, pdex++) {
    int aid = simem_cmpc(self, index, cptr[pdex]);
    if(aid) return aid; 
  }
  return 0;
}

/** Copies size bytes data from a void pointer to the SiMem, starting at index. 
Returns amount of bytes actually copied. */
int simem_putdata(SiMem * self, size_t index,  void * ptr, size_t size) {
  char * cptr = (char *) ptr;
  size_t room, pdex;
  if((!self) || (!ptr)) return 0;
  room = self->room;
  for(pdex = 0 ; (index < room) && (pdex < size); index ++, pdex++) {
    self->data[index] = cptr[pdex];
  }
  return pdex;
}

/** Copies size bytes data to a void pointer to the SiMem, starting at index. 
Returns amount of bytes actually copied. */
int simem_getdata(SiMem * self, size_t index,  void * ptr, size_t size) {
  char * cptr = (char *) ptr;
  size_t room, pdex;
  if((!self) || (!ptr)) return 0;
  room = self->room;
  for(pdex = 0 ; (index < room) && (pdex < size); index ++, pdex++) {
    cptr[pdex] = self->data[index];
  }
  return pdex;
}


/** Copies data from SiMem src to Simem dst, starting at srci and dsti,
copying size bytes.  Returns amount of bytes actually copied.  */
int simem_copy(SiMem * dst, size_t dsti, SiMem * src, size_t srci, size_t size)
{
  size_t srcroom, dstroom, index;  
  if(!simem_indexok(dst, dsti)) { return 0; }    
  if(!simem_indexok(src, srci)) { return 0; }
  srcroom = simem_room(src); 
  dstroom = simem_room(dst);
  for(  index = 0 ; 
        (srci < srcroom) && (dsti < dstroom) && (index < size); 
        index ++, srci++, dsti++) {
        dst->data[dsti] = src->data[srci];
  }
  return index;
}

/** Copies all data from src to dst starting at indexes 0,0 */
int simem_copyall(SiMem *dst, SiMem *src) {
  return simem_copy(dst, 0, src, 0, simem_room(src));
}

/** Fills a data block with the character value, starting at index, with size
size */
SiMem * simem_fill(SiMem * self, char value, size_t index, size_t size) {
  size_t room, pdex;
  if(!self) return NULL;
  room = simem_room(self);
  for(pdex = 0 ; (index < room) && (pdex < size); index ++, pdex++) {
    self->data[index] = value;
  }
  return self;
}

/** Fills a data block completely with the character value*/
SiMem * simem_fillall(SiMem * self, char value) {
  return simem_fill(self, value, 0, simem_room(self));
}


/** Duplicates a data block. */
SiMem * simem_dup(SiMem * self) {
  SiMem * res = simem_new(simem_room(self));
  if(!res) return NULL;
  simem_copyall(res, self);
  return res;
}

/** returns the head pointer of the memory block. */
size_t simem_ptr(SiMem * self) {
  return self->data;
}


/** Stores elemsize of data at index * elsize. */
void * simem_putelement(SiMem * self, size_t index, void * ptr, size_t elsize) {
  size_t real = index * elsize;
  if(!(simem_indexok(self, real + elsize - 1))) return NULL;
  simem_putdata(self, real, ptr, elsize);
  return ptr;
}

/** Gets a pointer to the data at index * elsize. */
void * simem_getelement(SiMem * self, size_t index, size_t elsize) {
  size_t real = index * elsize;
  if(!(simem_indexok(self, real))) return NULL;
  return self->data + (index * elsize);
}


/** Stores a pointer at index * sizeof(void *). */
void * simem_putptr(SiMem * self, size_t index, void * ptr) {
  size_t real = index * sizeof(ptr);
  if(!(simem_indexok(self, real + sizeof(ptr) - 1))) return NULL;
  simem_putdata(self, real, &ptr, sizeof(ptr));
  return ptr;
}

/** retrieves a pointer from the memeory block self at 
index * sizeof(void *). */
void * simem_getptr(SiMem * self, size_t index) {
  void * ptr;
  size_t real = index * sizeof(ptr);
  if(!(simem_indexok(self, real + sizeof(ptr) - 1))) return NULL;
  simem_getdata(self, real, &ptr, sizeof(ptr));
  return ptr;
}




#ifdef _COMMENT

#endif

/*
SiMem * simem_dup(SiMem * self);
SiMem * simem_copy(SiMem * self, SiMem * mem, size_t size);
SiMem * simem_xcopy(SiMem * self, SiMem * mem, size_t iself, size_t imem, size_t
size);
SiMem * simem_free(SiMem * self);

size_t simem_size(SiMem * self);
void * simem_ptr(SiMem * self);
*/



