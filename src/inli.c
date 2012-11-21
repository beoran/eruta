
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
* Parameters:
*   self - Inli to initialize
*   next - Next Inli list node to link to or NULL
*   prev - Previous Inli list node to link to or NULL
*
* Returns: 
*   self
*/
Inli * inli_initall(Inli * self, Inli * next, Inli * prev) {
  if (!self) return NULL;
  self->next = next;
  self->prev = prev;
  return self;
}

/**
* Function: inli_init
*
* Initializes the intrusive linked list. Next and prev are set to NULL.
*
* Parameters:
*   self - Inli to initialize
*
* Returns: 
*   self
*/
Inli * inli_init(Inli * self) {
  return inli_initall(self, NULL, NULL);
}

/** 
* Function: inli_remove
*
* Removes self from the list it is part of. 
* Does NOT clean up any data asssociated with the container of the intrusive 
* list and also doesn't free self, since self should be part of the container 
* of the intrusive list. 
*
* Parameters:
*   self - Inli to remove from whole of list
*
* Returns: 
*   self
*/
Inli * inli_remove(Inli * self) {  
  if(!self) return NULL;
  if(self->prev) { self->prev->next = self->next; }
  if(self->next) { self->next->prev = self->prev; }
  return self;
}

/** 
* Function: inli_add
*
* Appends other after self. 
*
* Parameters:
*   self - Inli to append to
*   other - Inli to append to self.
*
* Returns:
*   other if all went OK, NULL on error
*/
Inli * inli_add(Inli * self, Inli * other) {  
  if(!self || !other) return NULL;
  self->next  = other;
  other->prev = self;
  return other;
}

/** 
* Function: inli_next
*
* Parameters:
*   self - Inli
*
* Returns: 
*   the next element in the list, or NULL if no next item. 
*/
Inli * inli_next(Inli * self) {
  if(!self) return NULL;
  return self->next;
}

/** 
* Function: inli_next
*
* Parameters:
*   self - Inli
*
* Returns: 
*   the next previous in the list, or NULL if no previous item. 
*/
Inli * inli_prev(Inli * self) {
  if(!self) return NULL;
  return self->prev;  
}

/** 
* Function: inli_data
* 
* Parameters:
* self - list node to get the data of.
* offset - Use offsetof to calculate this offset.
*
* Returns:  
*   the data for this node of the singly linked list. 
* 
*/
void * inli_data(Inli * self, int offset) {
  return (void *)((char *)self - offset);
}


