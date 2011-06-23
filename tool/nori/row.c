
#define NORI_INTERN_ONLY
#include "nori_intern.h"


/** A row is an array. */
struct NoriRow_;
typedef struct NoriRow_ NoriRow;

struct NoriRow_ {
  void * ptr;
  NoriSize len;
  NoriSize cap;
  NoriSize esz;
};


/** Returns the length of the row.  */
NoriSize nori_row_len(NoriRow * row) {
  return row->len;
}

/** Returns the capacity of the row.  */
NoriSize nori_row_cap(NoriRow * row) {
  return row->cap;
}


/* Full in bytes of the buffer of the NoriRow. */
NoriSize nori_row_sizeof(NoriRow * row) {
  return row->cap * row->esz;  
}

/* Returns the pointer of the row as a (void *) pointer. */
void * nori_row_ptr(NoriRow * row) {
  return row->ptr;
}


/**
* Initializes an empty row, using the given buffer ptr as storage.
* ptr must point to an area of memory that has at least a capacity to store
* cap items of esz size each.
*/
NORI_FUNC(NoriRow *) nori_row_initptr
            (NoriRow * row, void * ptr, NoriSize cap, NoriSize esz) { 
  row->ptr      = ptr;
  row->cap      = cap;
  row->esz      = esz;
  row->len      = 0;
  // Ensure buffer is zero filled.
  memset(row->ptr, 0, nori_row_sizeof(row));  
  return row;
}

/**
* Initializes an empty row by allocating enough space for cap 
* elements of size esz each.
*/
NORI_FUNC(NoriRow *) nori_row_init(NoriRow * row, NoriSize cap, NoriSize esz) { 
  void * ptr      = NORI_MALLOC(cap * esz); 
  return nori_row_initptr(row, ptr, cap, esz);
}


/** Cleans up the contents of the row after use, although it does not call 
* any cleanup function on the elements themselves. 
*/
NORI_FUNC(NoriRow *) nori_row_done(NoriRow * row) {
  if(row->ptr) {
    NORI_FREE(row->ptr);
    row->ptr = NULL;
    row->cap = 0;
    row->len = 0;    
  } 
  return row;
}

/** Returns a pointer offset, taking element size into consideration */
uint8_t * nori_row_offset(NoriRow * row, uint8_t * ptr, int offset) {
  return ptr + (offset * row->esz);
}

/** Checks if the arguments are ok or not */
int nori_row_xmemcpy_ok(NoriRow * arr, NoriSize len, 
                      int astart, int pstart, NoriSize plen) { 
  
  if (astart < 0)                 { 
    fprintf(stderr, "Astart negative\n"); return FALSE; 
  } 
  if (pstart < 0)                 { 
    fprintf(stderr, "Pstart negative\n"); 
    return FALSE; 
  }
  if (len    < 0)                 { 
    fprintf(stderr, "Len negative\n");
    return FALSE; 
  }
  if ((astart + len) > arr->cap)  {
    fprintf(stderr, "NoriRow capacity too small.\n"); 
    return FALSE; 
  }
  if ((pstart + len) >  plen)     {
    fprintf(stderr, "Plen exeeded.\n"); 
    return FALSE; 
  }
  return TRUE;
}

/** 
* Copies len elements from ptr into the row, starting from 
* pstart, and astart as long as there is enough capacity in the row 
* to do so, and if it would not mean exeeding plen to do so.
*/
NORI_FUNC(NoriRow *) 
nori_row_xmemcpy(NoriRow *arr, const void * ptr, NoriSize len, 
                 int astart, int pstart, NoriSize plen) {
  int index, offset, memdex;
  uint8_t * src, * dst;
  uint8_t * ptr8 = (uint8_t *) ptr;
  // Check arguments and return NULL if out of range.   
  if(!nori_row_xmemcpy_ok(arr, len, astart, pstart, plen)) {
    return NULL;
  }

  src = nori_row_offset(arr, ptr8    , pstart);
  dst = nori_row_offset(arr, arr->ptr, astart);
  memmove(dst, src, arr->esz * len);
  // copy all the data in one block. 
  // set row length based on parameters
  if ( (len + astart ) > arr->len) { 
    arr->len  = (len + astart );
  }  
  return arr;
}  

/** 
* Copies len elements from row into the ptr, starting from 
* astart, and pstart as long as there is enough capacity in the row 
* to do so, and if it would not mean exeeding plen 
* do so.
*/
NORI_FUNC(NoriRow *) 
nori_row_xmemcpyto(NoriRow *arr, void * ptr, NoriSize len, 
                   int astart, int pstart, NoriSize plen) {
  int index, offset, memdex;
  uint8_t * src, * dst;
  uint8_t * ptr8 = (uint8_t *) ptr;
  // Check arguments and return NULL if out of range.   
  if(!nori_row_xmemcpy_ok(arr, len, astart, pstart, plen)) {
    return NULL;
  }
  src = nori_row_offset(arr, ptr8    , pstart);
  dst = nori_row_offset(arr, arr->ptr, astart);
  memmove(src, dst, arr->esz * len);
  return arr;
}  


/**
* Copies a C row from a memory location into the NoriRow.
*/
NORI_FUNC(NoriRow *) 
nori_row_memcpy(NoriRow * arr, const void * ptr, NoriSize plen) {
  return nori_row_xmemcpy(arr, ptr, plen, 0, 0, plen);
}

/** 
* Sets a value in the row at the given index.
* Returns null if it is out of range, returns row of all was ok. 
*/
NORI_FUNC(NoriRow *) 
nori_row_set(NoriRow * row, int index, const void *item) {
  // Copy the single item into the right place. 
  return nori_row_xmemcpy(row, item, 1, index, 0, 1); 
}

/** 
* Gets a value in the row at the given index, and stores it in 
* item.
* Returns null if it is out of range, returns row of all was ok. 
*/
NORI_FUNC(NoriRow *) nori_row_get(NoriRow * row, int index, void *item) {
  return nori_row_xmemcpyto(row, item, 1, index, 0, 1);
} 

/** 
* Fills an row upto it's capacity with the given pointed to value.
* The row's length will be set 
*/
NORI_FUNC(NoriRow *) nori_row_fill(NoriRow * row, void * item) {
  int index;
  for( index=0; index < nori_row_cap(row); index++) {
    nori_row_set(row, index, item);
  }
} 


/** Truncates an row to the given length by resetting it's length.
* Does not affect capacity. 
*/
NORI_FUNC(NoriRow *) nori_row_trunc(NoriRow * row, int size) { 
  if (size < 0)         { return NULL; }
  if (size >= row->cap) { return NULL; }
  row->len = size; 
}


/** 
* Concatenates rows together. dst will be modified if it's capacity 
* allows it. NoriRows must have same element size. 
*/
NORI_FUNC(NoriRow *) nori_row_cat(NoriRow * dst, NoriRow * src) {
  if (dst->esz != src->esz) {
    fprintf(stderr, "Element size differs!\n"); 
    return NULL; 
  }
  return nori_row_xmemcpy(dst, nori_row_ptr(src), nori_row_len(src),
                       nori_row_len(dst), 0, nori_row_len(src));  
}





