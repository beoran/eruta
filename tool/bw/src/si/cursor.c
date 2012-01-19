#include "si_mem.h"
#include "si_mem.h"
#include "si_cursor.h" 



struct SiCursor_ {
  SiCursorClass * klass;
  void * collection;
  void * active;
  size_t index;  
};

SiCursor * sicursor_init(SiCursor * self, void * collection, 
                         void * active  , size_t index,
                         SiCursorClass * klass) {
  if(!self) return NULL;
  self->collection  = collection;
  self->active      = active;
  self->index       = index;
  self->klass       = klass;
  return self;
}


SiCursor * sicursor_alloc() {
  return SI_ALLOCATE(SiCursor); 
}

SiCursor * sicursor_new(void * collection, void * active, 
                        size_t index, SiCursorClass * klass) {
  return sicursor_init(sicursor_alloc(), collection, active, index, klass);
}

SiCursor * sicursor_free(SiCursor * self) {
  return si_free(self);
}

void * sicursor_collection(SiCursor * self) {
  return self->collection;
}

void * sicursor_active(SiCursor * self) {
  return self->active;
}

void * sicursor_active_(SiCursor * self, void * active) {
  return self->active = active;
}

size_t sicursor_index(SiCursor * self) {
  return self->index;
}

size_t sicursor_index_(SiCursor * self, size_t index) {
  return self->index = index;
}

SiCursor * sicursor_next(SiCursor * self) {
  if(!self) return NULL;
  SiCursor * next = self->klass->next(self);
  if(next) { 
    next->index += 1; // increment the cursor's index
    return next;
  }  
  // If next is NULL, we're at the end of the iteration. 
  // Automatically free self and return null.
  return sicursor_free(self);
}

void * sicursor_data(SiCursor * self) {
  if(!self) return NULL;
  return self->klass->data(self);
}

void * sicursor_each(SiCursor * cursor, SiCursorFunc * func, void * extra) {
  do { 
    cursor = sicursor_next(cursor);
    if(cursor) { 
      void * aid = func(cursor, extra);
      if(aid) return aid; 
    }
  } while(cursor);
  return NULL;
}



