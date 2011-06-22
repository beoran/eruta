#include "si_mem.h"
#include "si_list.h"

/* A SiList is a (doubly) linked list. */

struct SiList_ {
  void   * data;
  SiList * next;
  SiList * back;
};


SiList * silist_linkto(SiList * self, SiList * next) {
  if(self) self->next = next;
  if(next) next->back = self;
  return self;
}


SiList * silist_init(SiList * self, void * data) {
  if(!self) return NULL;
  self->next = self->back = NULL;
  self->data = data; 
  return self;
};

SiList * silist_alloc() {
  return SI_ALLOCATE(SiList);
}

SiList * silist_new(void * data) {
  return silist_init(silist_alloc(), data);
};

void * silist_data(SiList * self) {
  if(!self) return NULL;
  return self->data;
}

SiList * silist_next(SiList * self) {
  if(!self) return NULL;
  return self->next;
}

SiList * silist_previous(SiList * self) {
  if(!self) return NULL;
  return self->back;
}

/** Frees self and self only. */
SiList * silist_free_self(SiList * self) {
  return si_free(self);
}

/** Frees self and all forwardly linked links. Does not free backwards links. */

SiList * silist_free(SiList * self) {
  silist_done(self);
  return si_free(self);
}

/** Frees all forwardly linked links only. */
SiList * silist_done(SiList * self) {
  SiList * index;
  for(index = silist_next(self); index; index = silist_next(index)) {
    silist_free_self(index);
  }
  return self;
}

/** Returns the last item of the list */
SiList * silist_last(SiList * self) {
  SiList * index = self;
  SiList * last  = NULL;
  do {
    last   = index;
    index  = silist_next(last);
  } while(index);
  return last;
}

/** Add a list to the end of a list */
SiList * silist_addlist(SiList * self, SiList * next) {
  SiList * last = silist_last(self);
  return silist_linkto(last, next);
}

/** Creates a new link, stores a pointer to data in it, 
and appends it to this list. */
SiList * silist_add(SiList * self, void * data) {
  SiList * next = silist_new(data);  
  return silist_addlist(self, next);
}


/** List's cursor's next method.*/
SiCursor * silist_cursor_next(SiCursor * self) {  
  SiList * active = (SiList *) sicursor_active(self);
  if(active->next) {
    sicursor_active_(self, active->next);
    return self;
  }
  return NULL;
}

/** List's cursor's data method.*/
void * silist_cursor_data(SiCursor * self) {
   SiList * active = (SiList *) sicursor_active(self);   
   return silist_data(active);
}

/** List's cursor class description. */
static SiCursorClass list_cursor_klass = {
  silist_cursor_next,
  silist_cursor_data,
};

SiCursor * silist_cursor(SiList * self) {
  return sicursor_new(self, self, 0,  &list_cursor_klass);
}

