

#include "pachi.h"

/** Initializes the parent child tree, sets the parent. */
Pachi * pachi_init(Pachi * self, Pachi * parent) {
  if(!self) return self;
  self->parent  = parent; 
  self->next    = NULL;
  self->child   = NULL;
  return self;
}

/** Adds next as the sibling or next node of self. */
Pachi * pachi_addnext(Pachi * self, Pachi * next) {
  Pachi aid = self;
  if(!self) return NULL;
  while(aid->next) { aid = aid->next }
  aid->next = next;
  return next;
}

/** Adds child as a child to self. */
Pachi * pachi_addchild(Pachi * self, Pachi * child) {
  Pachi * aid;
  if(!self) return NULL;
  if(!self->child) {
    self->child = child;
    return child;
  }
  return pachi_addnext(self->child, child);
}






