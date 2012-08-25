
#include "silli.h"
#include "mem.h"


/** Silli is a simple singly linked list. It does NOT own it's elements, 
and they must be erased manually before erasing the list. */
struct Silli_ {
  void          * data;
  struct Silli_ * next;
};


Silli * silli_alloc() {
  return STRUCT_ALLOC(Silli);
}


Silli * silli_initall(Silli * self, void * data, Silli * next) {
  if (!self) return NULL;
  self->next = next;
  self->data = data;
}

Silli * silli_init(Silli * self, void * data) {
  return silli_initall(self, data, NULL);
}

Silli * silli_newall(void * data, Silli * next) {
  return silli_initall(silli_alloc(), data, next);
}

/***/
Silli * silli_new(void * data) {
  return silli_init(silli_alloc(), data);
}

/** Frees the linked list and uses the given destructor to clean up the data 
too. Does not clean the data in the list if destroy is NULL. */
Silli * silli_free_destroy(Silli * self, MemDestructor * destroy) {
  Silli * next;
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
Silli * silli_free(Silli * self) {
  return silli_free_destroy(self, NULL);
}

/** Returns the next item in the singly linked list. */
Silli * silli_next(Silli * self) {
  return self->next;
}

/** Returns the data for this node of the singly linked list. */
Silli * silli_data(Silli * self) {
  return self->data;
}

/** Inserts the node two after th node one in the singly linked list. 
returns one. Does nothing if one is NULL. If two is null, the list is 
truncated mercilessly without even calling silli_free on it.
*/
Silli * silli_add(Silli * one, Silli * two) {
  Silli * old_next;
  if(!one) return NULL;
  old_next  = one->next;
  one->next = two;
  if(two) { 
    two->next = old_next;
  }
  return one;
}

/** Iterates over the list using the AllData interface callback */
Silli * silli_all(Silli * self, AllData * callback, void * extra) {
  Silli * index;
  void * result;
  if(!callback) return NULL;
  for(index = self; index ; index = index->next) {
    result = callback(index->data, extra);
    if(result) return result;
  }
  return NULL;
}

/** Tries to find to_find using the AllCompare callback. */
Silli * silli_find(Silli * self, AllCompare * callback, void * to_find) {
  Silli * index;
  int result;
  if(!callback) return NULL;
  for(index = self; index ; index = index->next) {
    result = callback(index->data, to_find);
    if(result == 0) return result;
  }
  return NULL;
}






