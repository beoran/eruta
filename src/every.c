#include "mem.h"
#include "every.h"



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








