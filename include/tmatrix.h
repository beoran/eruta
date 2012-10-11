

/* 
This file contains a template that can generate compile-time generic 
2d matrix strucst and functions. To use it, do as follows:

#define TMATRIX_T int
#define TMATRIX_NAME MyMatrix
#include tmatrix.h

Then in some C file, write: 

#define TMATRIX_T int
#define TMATRIX_NAME MyMatrix
#define TMATRIX_IMPLEMENT
#include tmatrix.h

Optionally the following template parameters may also be defined:
TMATRIX_ALLOC(SIZE) : custom malloc-compatible allocation function
TMATRIX_FREE(PTR)   : custom free-compatible allocation function
TMATRIX_PREFIX      : prefix for the functions gererated. Default: TMATRIX_NAME.


The technique used to implement this kind of ANSI C templates is also known as 
"X-Macro", since it relies on the availability certain macros that are yet 
undefined in this file and must be supplied by the programmer.

*/

/** Type of elememts. */
#ifndef TMATRIX_T
#error Please define TMATRIX_T to be the element type of the matrix
#endif

/** Name of generated struct. */
#ifndef TMATRIX_NAME
#error Please define TMATRIX_NAME to be the name of the type of the matrix
#endif

/* Prefix for generated function names. Defaults to TMATRIX_NAME. */
#ifndef TMATRIX_PREFIX
#define TMATRIX_PREFIX TMATRIX_NAME
#endif

#ifndef TMATRIX_ALLOC
#define TMATRIX_ALLOC(SIZE) calloc(SIZE, 1)
#endif 

#ifndef TMATRIX_FREE
#define TMATRIX_FREE(MEM) free(MEM)
#endif 

/* needed C library functions */
#include <stdlib.h>


/* Helper macros. */ 
#define TSTR_HELPER(S)      #S
#define TSTR(S)             TSTR_HELPER(S)
#define TJOIN_HELPER(A, B)  A##B
#define TJOIN(A,B)          TJOIN_HELPER(A,B)
#define TSTRUCT(NAME)       TJOIN(NAME, _struct)
#define TFUNC(NAME, FUNC)   TJOIN(NAME, FUNC)

/* Specific helper macros. */
#define TMATRIX_STRUCT      TSTRUCT(TMATRIX_NAME)
#define TMATRIX_FUNC(FUNC)  TFUNC(TMATRIX_PREFIX, FUNC)


/* Don't procede unless the essential template parameters are set. */
#if defined(TMATRIX_NAME) && defined(TMATRIX_T) 

#ifndef TMATRIX_IMPLEMENT

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif


/** Interface of the TMATRIX. */
struct TMATRIX_STRUCT {
  TMATRIX_T **data; 
  int         w;
  int         h;
};

typedef struct TMATRIX_STRUCT TMATRIX_NAME;


/** Allocates a new, uninitialized TMATRIX. */
TMATRIX_NAME * TMATRIX_FUNC(alloc)(void);

/** Cleans up and deinitializes the TMATRIX self  */
TMATRIX_NAME * TMATRIX_FUNC(done) (TMATRIX_NAME * self);

/** Calls TMATRIX_NAME_done, and then frees the TMATRIX self  */
TMATRIX_NAME * TMATRIX_FUNC(free) (TMATRIX_NAME * self);

/** Returns the width of the TMATRIX. Returns -1 is self isn't propery 
* initialized. 
*/
int TMATRIX_FUNC(w) (TMATRIX_NAME * self);

/** Returns the height of the TMATRIX. 
* Returns -1 is self isn't propery initialized. */
int TMATRIX_FUNC(h) (TMATRIX_NAME * self);


/** Initializes a TMATRIX to be of the given width and height. 
* Returns NULL on error, or if h or w are less than 1.
* Returns self on sucess.
*/
TMATRIX_NAME * TMATRIX_FUNC(init) (TMATRIX_NAME * self, int w, int h);
/**
* Return true if the arguments x and or y are out of range, or
* if self itself isn't properly initialized. Return false if OK.
*/
int TMATRIX_FUNC(outofrange) (TMATRIX_NAME * self, int w, int h);

/** Gets an element from the matrix. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
TMATRIX_T TMATRIX_FUNC(getraw) (TMATRIX_NAME * self, int x, int y);


/** Gets a row of elements from the matrix. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
TMATRIX_T * TMATRIX_FUNC(rowraw) (TMATRIX_NAME * self, int y);


/** Gets an element from the matrix self. 
* If all goes well, this function returns 0, and sets the value to
* (*result). 
* If w and h are out of range, or the matrix isn't correctly initialized, 
* does nothing and returns -1 in stead.
*/
int TMATRIX_FUNC(get) (TMATRIX_NAME * self, int w, int h, TMATRIX_T * result);

/** Puts an element in the matrix self. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
void TMATRIX_FUNC(setraw) (TMATRIX_NAME * self, int w, int h, TMATRIX_T el);

/** Puts and element info the matrix self. 
* If all goes well, this function returns 0.
* If w and h are out of range, or the matrix isn't correctly initialized, 
* does nothing and returns -1 in stead.
*/
int TMATRIX_FUNC(put) (TMATRIX_NAME * self, int w, int h, TMATRIX_T el);


/** Copies the data from one matrix to the other. 
* If the matrixes are of unequal size, copies the smallest of both 
* heights and widths.
* Returns 0 on succes, -1 on error. 
*/
int TMATRIX_FUNC(copy) (TMATRIX_NAME * self, TMATRIX_NAME * other);


#else
/* Implementation of the TMATRIX. */

/** Allocates a new, uninitialized TMATRIX. */
TMATRIX_NAME * TMATRIX_FUNC(alloc) (void) {
  return TMATRIX_ALLOC(sizeof(TMATRIX_NAME));
}

/** Cleans up and deinitializes the TMATRIX self  */
TMATRIX_NAME * TMATRIX_FUNC(done) (TMATRIX_NAME * self) {
  int index;
  if(!self) return NULL;
  if(!self->data) return self; 
  for (index = 0; index < self->h; index ++) {    
    if(self->data[index]) { TMATRIX_FREE(self->data[index]); }
  }
  
  self->w = -1;
  self->h = -1;  
  TMATRIX_FREE(self->data); 
  return self;
}

/** Calls TMATRIX_NAME_done, and then frees the TMATRIX self. Returns NULL. */
TMATRIX_NAME * TMATRIX_FUNC(free) (TMATRIX_NAME * self) {
  TMATRIX_FUNC(done)(self);
  return NULL;
}

/** Returns the width of the TMATRIX. Returns -1 is self isn't propery 
* initialized. 
*/
int TMATRIX_FUNC(w) (TMATRIX_NAME * self) {
  if(!self) return -1;
  return self->w;
}

/** Returns the height of the TMATRIX. 
* Returns -1 is self isn't propery initialized. */
int TMATRIX_FUNC(h) (TMATRIX_NAME * self) {
  if(!self) return -1;
  return self->h;
}


/** Initializes a TMATRIX to be of the given width and height. 
* Returns NULL on error, or if h or w are less than 1.
* Returns self on sucess.
*/
TMATRIX_NAME * TMATRIX_FUNC(init) (TMATRIX_NAME * self, int w, int h) {
  int index;
  if(!self) return NULL;
  self->w   = w;
  self->h   = h;
  self->data = NULL;
  if ((self->h <= 0) || (self->w <= 0)) { return NULL; }
  // Allocate y dimension
  self->data = (TMATRIX_T **) TMATRIX_ALLOC(sizeof(TMATRIX_T *) * self->h);
  if(!self->data) return NULL;
  // Now allocate the x dimensions. 
  for (index = 0; index < self->h; index ++) {
    self->data[index] = (TMATRIX_T *) TMATRIX_ALLOC(sizeof(TMATRIX_T) * self->w);
    if(!self->data[index]) return TMATRIX_FUNC(free)(self); 
  }
  return self;  
}

/**
* Return true if the arguments x and or y are out of range, or
* if self itself isn't properly initialized. Return false if OK.
*/
int TMATRIX_FUNC(outofrange) (TMATRIX_NAME * self, int x, int y) {
  if(!self)                               return TRUE;
  if(!self->data)                         return TRUE;
  if ((x <  0) || (y <  0))             { return TRUE; }
  if ((y >= self->h) || (x >= self->w)) { return TRUE; }
  return FALSE;
}

/** Gets an element from the matrix. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
TMATRIX_T TMATRIX_FUNC(getraw) (TMATRIX_NAME * self, int x, int y) {
  return self->data[y][x];
}


/** Gets a row of elements from the matrix. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
TMATRIX_T * TMATRIX_FUNC(rowraw) (TMATRIX_NAME * self, int y) {
  return self->data[y];
}


/** Gets an element from the matrix self. 
* If all goes well, this function returns 0, and sets the value to
* (*result). 
* If w and h are out of range, or the matrix isn't correctly initialized, 
* does nothing and returns -1 in stead.
*/
int TMATRIX_FUNC(get) (TMATRIX_NAME * self, int x, int y, TMATRIX_T * result) {
  TMATRIX_T * aid;
  if(TMATRIX_FUNC(outofrange)(self,x,y)) { return -1; }  
  (*result) = TMATRIX_FUNC(getraw)(self,x,y);
  return 0;  
}

/** Puts an element in the matrix self. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
void TMATRIX_FUNC(putraw) (TMATRIX_NAME * self, int x, int y, TMATRIX_T el) {
  self->data[y][x] = el;
}


/** Puts and element info the matrix self. 
* If all goes well, this function returns 0.
* If w and h are out of range, or the matrix isn't correctly initialized, 
* does nothing and returns -1 in stead.
*/
int TMATRIX_FUNC(put) (TMATRIX_NAME * self, int x, int y, TMATRIX_T el) {
  if(TMATRIX_FUNC(outofrange)(self,x,y)) { return -1; }  
  TMATRIX_FUNC(putraw)(self,x,y,el);
  return 0;
}


/** Copies the data from one matrix to the other. 
* If the matrixes are of unequal size, copies the smallest of both 
* heights and widths.
* Returns 0 on succes, -1 on error. 
*/
int TMATRIX_FUNC(copy) (TMATRIX_NAME * self, TMATRIX_NAME * other) {
  int ws, hs, wo, ho, w, h, x, y;
  if ((!self) || (!other))              return -1;
  if ((!self->data) || (!other->data))  return -1;
  ws = TMATRIX_FUNC(w)(self);
  hs = TMATRIX_FUNC(h)(self);  
  
  wo = TMATRIX_FUNC(w)(other);
  ho = TMATRIX_FUNC(h)(other);  
  // Copy only smallest of width and height.
  w  = (ws < wo ? ws : wo);
  h  = (hs < ho ? hs : ho);
  for(y = 0; y < h ; y++) {
    for(x = 0; x < w ; x++) {
      self->data[y][x] = other->data[y][x];
    }
  }
  return 0;
}


#endif

#endif

/** Finally clean up by undefining all defined macros. **/
#undef TMATRIX_T
#undef TMATRIX_NAME
#undef TMATRIX_PREFIX
#undef TMATRIX_ALLOC
#undef TMATRIX_FREE
#undef TSTR_HELPER
#undef TSTR
#undef TJOIN_HELPER
#undef TJOIN
#undef TSTRUCT
#undef TFUNC
#undef TMATRIX_STRUCT
#undef TMATRIX_FUNC
#undef TMATRIX_IMPLEMENT








