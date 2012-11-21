
#include "inli.h"

Inli * inli_initall(Inli * self, Inli * next, Inli * prev) {
  if (!self) return NULL;
  self->next = next;
  self->prev = prev;
  return self;
}

Inli * inli_init(Inli * self) {
  return inli_initall(self, NULL, NULL);
}

/** Removes self from the list it is part of. 
* Does NOT clean up any data asssociated with 
* the container of the intrusive list. 
*/

Inli * inli_destroy(Inli * self, MemDestructor * destroy) {  

}

/** Frees the linked list and uses the given destructor to clean up the data 
too. Does not clean the data in the list if destroy is NULL. */
Inli * silli_free_destroy(Inli * self, MemDestructor * destroy) {
  Inli * next;
  next = self->next;
  while(next) {
    if(destroy) {
      destroy(self->data);
    }
    mem_free(self);
    self = next;
    next = self->next;
  }
  return NULL;
}

/** Frees the whole linked list from self on (and all folllowing nodes) 
but does NOT free it's elements. */
Inli * silli_free(Inli * self) {
  return silli_free_destroy(self, NULL);
}


/** Returns the next item in the singly linked list. */
Inli * silli_next(Inli * self) {
  return self->next;
}

/** Returns the data for this node of the singly linked list. */
Inli * silli_data(Inli * self) {
  return self->data;
}

/** Inserts the node two after th node one in the singly linked list. 
returns one. Does nothing if one is NULL. If two is null, the list is 
truncated mercilessly without even calling silli_free on it.
*/
Inli * silli_add(Inli * one, Inli * two) {
  Inli * old_next;
  if(!one) return NULL;
  old_next  = one->next;
  one->next = two;
  if(two) { 
    two->next = old_next;
  }
  return one;
}

/** Iterates over the list using the AllData interface callback */
Inli * silli_all(Inli * self, AllData * callback, void * extra) {
  Inli * index;
  void * result;
  if(!callback) return NULL;
  for(index = self; index ; index = index->next) {
    result = callback(index->data, extra);
    if(result) return result;
  }
  return NULL;
}

/** Tries to find to_find using the AllCompare callback. */
Inli * silli_find(Inli * self, AllCompare * callback, void * to_find) {
  Inli * index;
  int result;
  if(!callback) return NULL;
  for(index = self; index ; index = index->next) {
    result = callback(index->data, to_find);
    if(result == 0) return result;
  }
  return NULL;
}







