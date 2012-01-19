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
  return silist_free_self(self);
}

/** Unlinks the node in the list after self, and links self to the
next-nextnode if any. Returns a pointer to the node thus removed, or NULL if
no such node.*/
SiList * silist_unlinknext(SiList * self) {
  SiList * nextnext = NULL;  
  SiList * next     = silist_next(self);
  if(!next) return NULL;
  nextnext = silist_next(next);
  silist_linkto(self, nextnext);
  return next;  
}

/** Unlinks the next node and frees it (and it alone) */
SiList * silist_freenext(SiList * self) {
  return silist_free_self(silist_unlinknext(self)); 
}

/** Frees all forwardly linked links only. */
SiList * silist_done(SiList * self) {
  SiList * next   = NULL; 
  SiList * remove = silist_unlinknext(self);
  while(remove) {
    silist_free_self(remove);
    remove = silist_unlinknext(self);
  }
  self->next = NULL;
  self->back = NULL;   
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

/** Add a list to the end of a list. Returns next.  */
SiList * silist_addlist(SiList * self, SiList * next) {
  SiList * last = silist_last(self);
  silist_linkto(last, next);
  return next;
}

/** Creates a new link, stores a pointer to data in it, 
and appends it to this list. Returns the newly added wrapping list. */
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

/** Generate a new cursor. */
SiCursor * silist_cursor(SiList * self) {
  return sicursor_new(self, self, 0,  &list_cursor_klass);
}

/** Gets the index-th element in the list. This is an O(index) operation.
* Returns NULL if the list does not have index items in it.  
*/
SiList * silist_getlist(SiList * self, size_t index) {
  size_t   sub  = 0;
  SiList * next = self;
  while(next) {
    if(sub == index) return next;
    next = silist_next(next);
    index ++;
  }
  return NULL;
}

/** Gets the data of index-th element in the list. This is an O(index)
operation. Returns NULL if the list does not have index items in it. */
void * silist_get(SiList * self, size_t index) {
  return silist_data(silist_getlist(self, index));
}

/** Walks over the list using the walker interface. */
void * silist_walk(SiList * self, SiWalker * walker, void * extra) {
  SiList * index = self;
  while(index) {
    void * data = silist_data(index);
    void * aid  = walker(data, index, self, extra);
    if (aid) return aid;
    index = silist_next(index);
  }  
  return NULL;
}


