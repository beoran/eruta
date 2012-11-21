
#include "inli.h"
#include <stdlib.h>

/*
  Title: Intrusive Linked Lists
*/


/**
* Struct: Inli
*
* Intrusive linked lists.
*/

/**
* Function: inli_initall
*
* Fully initializes a non-NULL intrusive linked list.
*
* Returns: self
*/
Inli * inli_initall(Inli * self, Inli * next, Inli * prev) {
  if (!self) return NULL;
  self->next = next;
  self->prev = prev;
  return self;
}

/**
* Initializes the intrusive linked list. Next and prev are set to NULL.
*
* Returns: self
*/
Inli * inli_init(Inli * self) {
  return inli_initall(self, NULL, NULL);
}

/** Removes self from the list it is part of. 
* Does NOT clean up any data asssociated with 
* the container of the intrusive list and also doesn't free self,
* since it should be part of the container of the intrusive list. 
*/
Inli * inli_remove(Inli * self) {  
  if(!self) return NULL;
  if(self->prev) { self->prev->next = self->next; }
  if(self->next) { self->next->prev = self->prev; }
  return self;
}

/** Appends other after self. Returns other, or null if either self or 
other are NULL.  
*/
Inli * inli_add(Inli * self, Inli * other) {  
  if(!self || !other) return NULL;
  self->next  = other;
  other->prev = self;
  return other;
}

/** Returns the next element in the list. */
Inli * inli_next(Inli * self) {
  if(!self) return NULL;
  return self->next;
}

/** Returns the previous element in the list. */
Inli * inli_prev(Inli * self) {
  if(!self) return NULL;
  return self->prev;  
}

/** Returns the data for this node of the singly linked list. 
* Use offsetof to calculate offset.
*/
void * inli_data(Inli * self, int offset) {
  return (void *)((char *)self - offset);
}


