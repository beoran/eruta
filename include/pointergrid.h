#ifndef pointergrid_H_INCLUDED
#define pointergrid_H_INCLUDED

#include "mem.h"

/* A PointerGrid is a non-intrusive two dimensional array that can store pointers. The pointed-to data is not "owned" by the PointerGrid struct,
when setting or deleting the caller must do the needed cleanup. */

/** Interface of the TMATRIX. */
struct PointerGrid_ {
  void  ***data;
  int         w;
  int         h;
};

typedef struct PointerGrid_ PointerGrid;


/** Allocates a new, uninitialized TMATRIX. */
PointerGrid * pointergrid_alloc(void);

/** Cleans up and deinitializes the TMATRIX self  */
PointerGrid * pointergrid_done(PointerGrid * self, MemDestructor * destroy);

/** Calls PointerGrid_done, and then frees the TMATRIX self  */
PointerGrid * pointergrid_free(PointerGrid * self);

/** Returns the width of the TMATRIX. Returns -1 is self isn't propery 
* initialized. 
*/
int pointergrid_w(PointerGrid * self);

/** Returns the height of the TMATRIX. 
* Returns -1 if self isn't propery initialized. */
int pointergrid_h(PointerGrid * self);


/** Initializes a TMATRIX to be of the given width and height. 
* Returns NULL on error, or if h or w are less than 1.
* Returns self on sucess.
*/
PointerGrid * pointergrid_init(PointerGrid * self, int w, int h);

/** Allocates a new matrix and initializes it. */
PointerGrid * pointergrid_new(int w, int h);

/**
* Return true if the arguments x and or y are out of range, or
* if self itself isn't properly initialized. Return false if OK.
*/
int pointergrid_outofrange(PointerGrid * self, int w, int h);

/** Gets an element from the matrix. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
void * pointergrid_getraw(PointerGrid * self, int x, int y);


/** Gets a row of elements from the matrix. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
void ** pointergrid_rowraw(PointerGrid * self, int y);


/** Gets an element from the matrix self. 
* If all goes well, this function returns 0, and sets the value to
* (*result). 
* If w and h are out of range, or the matrix isn't correctly initialized, 
* does nothing and returns -1 in stead.
*/
int pointergrid_get(PointerGrid * self, int w, int h, void * * result);

/** Puts an element in the matrix self. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
void pointergrid_setraw(PointerGrid * self, int w, int h, void * el);

/** Puts and element info the matrix self. 
* If all goes well, this function returns 0.
* If w and h are out of range, or the matrix isn't correctly initialized, 
* does nothing and returns -1 in stead.
*/
int pointergrid_put(PointerGrid * self, int w, int h, void * el);

void pointergrid_putraw(PointerGrid * self, int x, int y, void * el);

/** Copies the data from one matrix to the other. 
* If the matrixes are of unequal size, copies the smallest of both 
* heights and widths.
* Returns 0 on succes, -1 on error. 
*/
int pointergrid_copy(PointerGrid * self, PointerGrid * other);





#endif




