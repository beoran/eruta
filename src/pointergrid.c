#include "pointergrid.h"


/** Allocates a new, uninitialized PointerGrid. */
PointerGrid * pointergrid_alloc(void) {
  return mem_alloc(sizeof(PointerGrid));
}


/** Nulls out all the data in pointergrid.  If memdestructor is not NULL,
 * it will be called with the old value in the cell before NULL is set.
*/
int pointergrid_nullall(PointerGrid * self, MemDestructor * destroy) {
  int w, h, x, y;
  if (!self)        return -1;
  if (!self->data)  return -1;
  w = pointergrid_w(self);
  h = pointergrid_h(self);  
  for(y = 0; y < h ; y++) {
    for(x = 0; x < w ; x++) {
      void * cell = pointergrid_getraw(self, x, y);
      if (destroy && cell) {
        destroy(cell); 
      }
      pointergrid_putraw(self, x, y, NULL);
    }
  }
  return 0;
}



/** Cleans up and deinitializes the PointerGrid self, 
using the destructor if needed  */
PointerGrid * pointergrid_done(PointerGrid * self, MemDestructor * destroy) {
  int index;
  if(!self) return NULL;
  if(!self->data) return self; 
  for (index = 0; index < self->h; index ++) {    
    if(self->data[index]) { 
      /* Clean up elements if needed. */
      if(destroy) {
        pointergrid_nullall(self, destroy);
      }
      mem_free(self->data[index]);
    }
    self->data[index] = NULL;
  }
  
  self->w = -1;
  self->h = -1;  
  mem_free(self->data);
  return self;
}

/** Calls PointerGrid_done, and then frees the PointerGrid self. Returns NULL. */
PointerGrid * pointergrid_free(PointerGrid * self) {
  pointergrid_done(self, NULL);
  mem_free(self);
  return NULL;
}

/** Returns the width of the PointerGrid. Returns -1 is self isn't propery 
* initialized. 
*/
int pointergrid_w(PointerGrid * self) {
  if(!self) return -1;
  return self->w;
}

/** Returns the height of the PointerGrid. 
* Returns -1 is self isn't propery initialized. */
int pointergrid_h(PointerGrid * self) {
  if(!self) return -1;
  return self->h;
}


/** Initializes a PointerGrid to be of the given width and height. 
* Returns NULL on error, or if h or w are less than 1.
* Returns self on sucess.
*/
PointerGrid * pointergrid_init(PointerGrid * self, int w, int h) {
  int index;
  if(!self) return NULL;
  self->w   = w;
  self->h   = h;
  self->data = NULL;
  if ((self->h <= 0) || (self->w <= 0)) { return NULL; }
  // Allocate y dimension
  self->data = (void * **) mem_alloc(sizeof(void * *) * self->h);
  if(!self->data) return NULL;
  // Now allocate the x dimensions. 
  for (index = 0; index < self->h; index ++) {
    self->data[index] = (void * *) mem_alloc(sizeof(void *) * self->w);
    if(!self->data[index]) return pointergrid_free(self); 
  }
  return self;  
}

/** Allocates a new matrix and initializes it. */
PointerGrid * pointergrid_new(int w, int h) {
  PointerGrid * self = pointergrid_alloc();
  return pointergrid_init(self, w, h);
}  
  

/**
* Return true if the arguments x and or y are out of range, or
* if self itself isn't properly initialized. Return false if OK.
*/
int pointergrid_outofrange(PointerGrid * self, int x, int y) {
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
void * pointergrid_getraw(PointerGrid * self, int x, int y) {
  return self->data[y][x];
}


/** Gets a row of elements from the matrix. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
void * * pointergrid_rowraw(PointerGrid * self, int y) {
  return self->data[y];
}


/** Gets an element from the matrix self. 
* If all goes well, this function returns 0, and sets the value to
* (*result). 
* If w and h are out of range, or the matrix isn't correctly initialized, 
* does nothing and returns -1 in stead.
*/
int pointergrid_get(PointerGrid * self, int x, int y, void * * result) {
  void * * aid;
  if(pointergrid_outofrange(self,x,y)) { return -1; }  
  (*result) = pointergrid_getraw(self,x,y);
  return 0;  
}

/** Puts an element in the matrix self. Does no checking whatsoever. 
* Use only where the inputs are guaranteed to be valid, where speed is 
* of the essence.
*/
void pointergrid_putraw(PointerGrid * self, int x, int y, void * el) {
  self->data[y][x] = el;
}


/** Puts and element info the matrix self. 
* If all goes well, this function returns 0.
* If w and h are out of range, or the matrix isn't correctly initialized, 
* does nothing and returns -1 in stead.
*/
int pointergrid_put(PointerGrid * self, int x, int y, void * el) {
  if(pointergrid_outofrange(self,x,y)) { return -1; }  
  pointergrid_putraw(self,x,y,el);
  return 0;
}


/** Sets and element info the matrix self. 
* If all goes well, this function returns el.
* If w and h are out of range, or the matrix isn't correctly initialized, 
* does nothing and returns NULL in stead.
*/
void * pointergrid_store(PointerGrid * self, int x, int y, void * el) {
  if(pointergrid_outofrange(self,x,y)) { return NULL; }  
  pointergrid_putraw(self,x,y,el);
  return el;
}

/** Fetched an element from the matrix self. 
* If all goes well, this returns the result.
* If w and h are out of range, or the matrix isn't correctly initialized, 
* returns NULL. 
*/
void * pointergrid_fetch(PointerGrid * self, int x, int y) {
  if (pointergrid_outofrange(self,x,y)) { return NULL; }  
  return pointergrid_getraw(self,x,y);
}

/** Copies the data from one matrix to the other. 
* If the matrixes are of unequal size, copies the smallest of both 
* heights and widths.
* Returns 0 on succes, -1 on error. 
*/
int pointergrid_copy(PointerGrid * self, PointerGrid * other) {
  int ws, hs, wo, ho, w, h, x, y;
  if ((!self) || (!other))              return -1;
  if ((!self->data) || (!other->data))  return -1;
  ws = pointergrid_w(self);
  hs = pointergrid_h(self);  
  
  wo = pointergrid_w(other);
  ho = pointergrid_h(other);  
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

