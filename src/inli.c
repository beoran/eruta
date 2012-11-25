
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
  self->prev = NULL;
  self->next = NULL;
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
* Returns the next element in the list
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
* Function: inli_prev
*
* Returns the previous element in the list
*
* Parameters:
*   self - Inli
*
* Returns: 
*   the next element in the list, or NULL if no next item. 
*/
Inli * inli_prev(Inli * self) {
  if(!self) return NULL;
  return self->prev;
}

/** 
* Function: inli_first
*
* Returns the first element in the list, by dumb iteration.
*
* Parameters:
*   self - Inli
*
* Returns: 
*   the first link in the list, or NULL if self is NULL. 
*/
Inli * inli_first(Inli * self) {
  Inli * aid = self; 
  if(!aid) return NULL;
  while (aid->prev) {
    aid = aid->prev;
  }
  return aid;  
}

/** 
* Function: inli_last
*
* Returns the last element in the list, by dumb iteration.
*
* Parameters:
*   self - Inli
*
* Returns: 
*   the last link in the list, or NULL if self is NULL. 
*/
Inli * inli_last(Inli * self) {
  Inli * aid = self; 
  if(!aid) return NULL;
  while (aid->next) {
    aid = aid->next;
  }
  return aid;  
}

/** 
* Function: inli_push
*
* Appends other to the end of the list by dumb iteration.
*
* Parameters:
*   self - Inli
*
* Returns: 
*   other, or NULL if self or other is NULL. 
*/
Inli * inli_push(Inli * self, Inli * other) {  
  Inli * aid;
  aid = inli_last(self);
  return inli_add(aid, other);
}


/** 
* Function: inli_unshift 
*                
* Prepends other to the beginning of the list by dumb iteration.
*
* Parameters:
*   self - Inli
*
* Returns: 
*   other, or NULL if self or other is NULL. 
*/
Inli * inli_unshift(Inli * self, Inli * other) {  
  Inli * aid;
  aid = inli_first(self);
  return inli_add(other, self);
}


/** 
* Function: inli_shift
*
* Removes the first element from the list by dumb iteration.
*
* Parameters:
*   self - Inli
*
* Returns: 
*   list node that was removed, or NULL if self is NULL. 
*/
Inli * inli_shift(Inli * self) {  
  Inli * aid;
  aid = inli_first(self);
  return inli_remove(aid);
}


/** 
* Function: inli_pop
*
* Removes the last element from the list by dumb iteration.
*
* Parameters:
*   self - Inli
*
* Returns: 
*   list node that was removed, or NULL if self is NULL. 
*/
Inli * inli_pop(Inli * self) {  
  Inli * aid;
  aid = inli_last(self);
  return inli_remove(aid);
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
  if(!self) return NULL;
  return (void *)((char *)self - offset);
}




