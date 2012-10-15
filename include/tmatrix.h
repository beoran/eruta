

/* 
This file contains a template that can generate compile-time generic 
2d matrix strucst and functions. To use it, do as follows:

#define TEMPLATE_T int
#define TEMPLATE_NAME MyMatrix
#include tmatrix.h

Then in some C file, write: 

#define TEMPLATE_T int
#define TEMPLATE_NAME MyMatrix
#define TEMPLATE_IMPLEMENT
#include tmatrix.h

Optionally the following template parameters may also be defined:
TEMPLATE_ALLOC(SIZE) : custom malloc-compatible allocation function
TEMPLATE_FREE(PTR)   : custom free-compatible allocation function
TEMPLATE_PREFIX      : prefix for the functions gererated. Default: TEMPLATE_NAME.


The technique used to implement this kind of ANSI C templates is also known as 
"X-Macro", since it relies on the availability certain macros that are yet 
undefined in this file and must be supplied by the programmer.

*/

#include <stdlib.h>

#include <tbegin.h>

#ifdef TEMPLATE_OK

#ifndef TEMPLATE_IMPLEMENT

/** Interface of the TMATRIX. */
struct TEMPLATE_STRUCT {
  TEMPLATE_T **data; 
  int         w;
  int         h;
};

typedef struct TEMPLATE_STRUCT TEMPLATE_NAME;


/** Allocates a new, uninitialized TMATRIX. */
TEMPLATE_NAME * TEMPLATE_FUNC(alloc)(void);

/** Cleans up and deinitializes the TMATRIX self  */
TEMPLATE_NAME * TEMPLATE_FUNC(done) (TEMPLATE_NAME * self);

/** Calls TEMPLATE_NAME_done, and then frees the TMATRIX self  */
TEMPLATE_NAME * TEMPLATE_FUNC(free) (TEMPLATE_NAME * self);

/** Returns the width of the TMATRIX. Returns -1 is self isn't propery 
* initialized. 
*/
int TEMPLATE_FUNC(w) (TEMPLATE_NAME * self);

/** Returns the height of the TMATRIX. 
* Returns -1 is self isn't propery initialized. */
int TEMPLATE_FUNC(h) (TEMPLATE_NAME * self);


/** Initializes a TMATRIX to be of the given width and height. 
* Returns NULL on error, or if h or w are less than 1.
* Returns self on sucess.
*/
TEMPLATE_NAME * TEMPLATE_FUNC(init) (TEMPLATE_NAME * self, int w, int h);

/** Allocates a new matrix and initializes it. */
TEMPLATE_NAME * TEMPLATE_FUNC(new) (int w, int h);

/**
* Return true if the arguments x and or y are out of range, or
* if self itself isn't properly initialized. Return false if OK.
*/
int TEMPLATE_FUNC(outofrange) (TEMPLATE_NAME * self, int w, int h);

/** Gets an element from the matrix. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
TEMPLATE_T TEMPLATE_FUNC(getraw) (TEMPLATE_NAME * self, int x, int y);


/** Gets a row of elements from the matrix. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
TEMPLATE_T * TEMPLATE_FUNC(rowraw) (TEMPLATE_NAME * self, int y);


/** Gets an element from the matrix self. 
* If all goes well, this function returns 0, and sets the value to
* (*result). 
* If w and h are out of range, or the matrix isn't correctly initialized, 
* does nothing and returns -1 in stead.
*/
int TEMPLATE_FUNC(get) (TEMPLATE_NAME * self, int w, int h, TEMPLATE_T * result);

/** Puts an element in the matrix self. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
void TEMPLATE_FUNC(setraw) (TEMPLATE_NAME * self, int w, int h, TEMPLATE_T el);

/** Puts and element info the matrix self. 
* If all goes well, this function returns 0.
* If w and h are out of range, or the matrix isn't correctly initialized, 
* does nothing and returns -1 in stead.
*/
int TEMPLATE_FUNC(put) (TEMPLATE_NAME * self, int w, int h, TEMPLATE_T el);


/** Copies the data from one matrix to the other. 
* If the matrixes are of unequal size, copies the smallest of both 
* heights and widths.
* Returns 0 on succes, -1 on error. 
*/
int TEMPLATE_FUNC(copy) (TEMPLATE_NAME * self, TEMPLATE_NAME * other);


#else
/* Implementation of the TMATRIX. */

/** Allocates a new, uninitialized TMATRIX. */
TEMPLATE_NAME * TEMPLATE_FUNC(alloc) (void) {
  return TEMPLATE_ALLOC(sizeof(TEMPLATE_NAME));
}

/** Cleans up and deinitializes the TMATRIX self  */
TEMPLATE_NAME * TEMPLATE_FUNC(done) (TEMPLATE_NAME * self) {
  int index;
  if(!self) return NULL;
  if(!self->data) return self; 
  for (index = 0; index < self->h; index ++) {    
    if(self->data[index]) { TEMPLATE_FREE(self->data[index]); }
  }
  
  self->w = -1;
  self->h = -1;  
  TEMPLATE_FREE(self->data); 
  return self;
}

/** Calls TEMPLATE_NAME_done, and then frees the TMATRIX self. Returns NULL. */
TEMPLATE_NAME * TEMPLATE_FUNC(free) (TEMPLATE_NAME * self) {
  TEMPLATE_FUNC(done)(self);
  return NULL;
}

/** Returns the width of the TMATRIX. Returns -1 is self isn't propery 
* initialized. 
*/
int TEMPLATE_FUNC(w) (TEMPLATE_NAME * self) {
  if(!self) return -1;
  return self->w;
}

/** Returns the height of the TMATRIX. 
* Returns -1 is self isn't propery initialized. */
int TEMPLATE_FUNC(h) (TEMPLATE_NAME * self) {
  if(!self) return -1;
  return self->h;
}


/** Initializes a TMATRIX to be of the given width and height. 
* Returns NULL on error, or if h or w are less than 1.
* Returns self on sucess.
*/
TEMPLATE_NAME * TEMPLATE_FUNC(init) (TEMPLATE_NAME * self, int w, int h) {
  int index;
  if(!self) return NULL;
  self->w   = w;
  self->h   = h;
  self->data = NULL;
  if ((self->h <= 0) || (self->w <= 0)) { return NULL; }
  // Allocate y dimension
  self->data = (TEMPLATE_T **) TEMPLATE_ALLOC(sizeof(TEMPLATE_T *) * self->h);
  if(!self->data) return NULL;
  // Now allocate the x dimensions. 
  for (index = 0; index < self->h; index ++) {
    self->data[index] = (TEMPLATE_T *) TEMPLATE_ALLOC(sizeof(TEMPLATE_T) * self->w);
    if(!self->data[index]) return TEMPLATE_FUNC(free)(self); 
  }
  return self;  
}

/** Allocates a new matrix and initializes it. */
TEMPLATE_NAME * TEMPLATE_FUNC(new) (int w, int h) {
  TEMPLATE_NAME * self = TEMPLATE_FUNC(alloc)();
  return TEMPLATE_FUNC(init)(self, w, h);
}  
  

/**
* Return true if the arguments x and or y are out of range, or
* if self itself isn't properly initialized. Return false if OK.
*/
int TEMPLATE_FUNC(outofrange) (TEMPLATE_NAME * self, int x, int y) {
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
TEMPLATE_T TEMPLATE_FUNC(getraw) (TEMPLATE_NAME * self, int x, int y) {
  return self->data[y][x];
}


/** Gets a row of elements from the matrix. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
TEMPLATE_T * TEMPLATE_FUNC(rowraw) (TEMPLATE_NAME * self, int y) {
  return self->data[y];
}


/** Gets an element from the matrix self. 
* If all goes well, this function returns 0, and sets the value to
* (*result). 
* If w and h are out of range, or the matrix isn't correctly initialized, 
* does nothing and returns -1 in stead.
*/
int TEMPLATE_FUNC(get) (TEMPLATE_NAME * self, int x, int y, TEMPLATE_T * result) {
  TEMPLATE_T * aid;
  if(TEMPLATE_FUNC(outofrange)(self,x,y)) { return -1; }  
  (*result) = TEMPLATE_FUNC(getraw)(self,x,y);
  return 0;  
}

/** Puts an element in the matrix self. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
void TEMPLATE_FUNC(putraw) (TEMPLATE_NAME * self, int x, int y, TEMPLATE_T el) {
  self->data[y][x] = el;
}


/** Puts and element info the matrix self. 
* If all goes well, this function returns 0.
* If w and h are out of range, or the matrix isn't correctly initialized, 
* does nothing and returns -1 in stead.
*/
int TEMPLATE_FUNC(put) (TEMPLATE_NAME * self, int x, int y, TEMPLATE_T el) {
  if(TEMPLATE_FUNC(outofrange)(self,x,y)) { return -1; }  
  TEMPLATE_FUNC(putraw)(self,x,y,el);
  return 0;
}


/** Copies the data from one matrix to the other. 
* If the matrixes are of unequal size, copies the smallest of both 
* heights and widths.
* Returns 0 on succes, -1 on error. 
*/
int TEMPLATE_FUNC(copy) (TEMPLATE_NAME * self, TEMPLATE_NAME * other) {
  int ws, hs, wo, ho, w, h, x, y;
  if ((!self) || (!other))              return -1;
  if ((!self->data) || (!other->data))  return -1;
  ws = TEMPLATE_FUNC(w)(self);
  hs = TEMPLATE_FUNC(h)(self);  
  
  wo = TEMPLATE_FUNC(w)(other);
  ho = TEMPLATE_FUNC(h)(other);  
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
#include <tend.h>








