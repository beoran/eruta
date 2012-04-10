#include "mem.h"
#include "every.h"

/** This file contains two iterator or iterator interfaces,
namely Every and Each. Every uses a struct to
keep track of it's state, while Each uses a callback function. */

/** Allocates an uninitialized every object. */
Every * every_alloc() {
  return STRUCT_ALLOC(Every);
}

/** Deinitializes an every object.  */
Every * every_done(Every * self) {
  if(!self) return NULL;
return self;
}

/** Frees an every object. */
Every * every_free(Every * self) {
  every_done(self);
  mem_free(self);
  return NULL;
}

/** Initializes an every object. */
Every * every_init(Every * self, EveryActs * acts) {
  if(!self) return NULL;
  self->acts    = acts;
  if(self->acts->init)
    return self->acts->init(self);
  return self;
}

/** Creates a new every object. */
Every * every_new(EveryActs * acts) {
  Every * self = every_alloc();
  return every_init(self, acts);
} 

/** Gets the current object pointed to by the Every object. */
void * every_get(Every * self) {
  if(!self) return NULL;
  return self->now;
}

/** Puts an object at the current position if possible.
* Returns NULL if the put failed.
*/
void * every_put(Every * self, void * data) {
  if(!self) return NULL;
  if(self->acts->put) {
    return self->acts->put(self, data);
  }
  return NULL;
}

/** Moves on to the next object and returns it.
* Return NULL if no next object.
*/
Every * every_next(Every * self, void * data) {
  if(self->acts->next) {
    self->acts->next(self);
    return every_get(self);
  }
  return NULL;
}


/** Initializes an EachElement */
Each * each_init(Each * self, void * on, void * data) {
  if(!self) return NULL;
  self->on    = on;
  self->extra = data;
  self->now   = NULL;
  self->index = -1;  
  return self;
}

/** Moves on to next element, incrementing index. */
Each * each_next(Each * self, void * now) {
  if(!self) return NULL;
  self->now = now;
  self->index++;
  return self;
}

/** Gets now pointer of each struct */
void * each_now(Each * self) {
  if(!self) return NULL;
  return self->now;
}  

/** Gets on pointer of each struct */
void * each_on(Each * self) {
  if(!self) return NULL;
  return self->on;
}

/** Gets extra data pointer of each struct */
void * each_extra(Each * self) {
  if(!self) return NULL;
  return self->extra;
}

/** Gets index pointer of each struct, -1 if the struct is NULL. */
int each_index(Each * self) {
  if(!self) return -1;
  return self->index;
}







