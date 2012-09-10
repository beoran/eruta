
#include "obj.h"


/* Hidden object header that implements the reference counting. */
struct ObjHeader_ {
  struct ObjClass_  * klass;
  int                 refcount;
};

typedef struct ObjHeader_ ObjHeader;


struct ObjPoolNode_ {
  // linked list to next object in the pool
  ObjPoolNode       * next;
  // object registered with the pool
  void *            * data; 
};


/** Initializes an object pool for use */
ObjPool * objpool_init(ObjPool * pool) {
  if(!pool) return NULL;
  pool->last = NULL;
  return pool;
}




/* Allocates a new object pool node for the given object header. */
static ObjPoolNode * objpoolnode_new(ObjPoolNode * next, void * data) {
  ObjPoolNode * self  = STRUCT_ALLOC(ObjPoolNode);
  if(!self) return NULL;
  self->next          = next;
  self->data          = data;
  return self;
}


/** Registers data to be used with the given pool. */
void * objpool_register_data(ObjPool * pool, void * data) {
  ObjPoolNode * node;
  if(!pool) return NULL;
  // the next in the linked list will be the last registered object of the pool 
  node = objpoolnode_new(pool->last, data);
  if(!node) return NULL;
  // now the last in the pool must be the registered object
  pool->last = node;
  return data;
}



/** Allocates an object with the given size in the given pool. 
This will work much like mam_alloc / calloc, except that 
a hidden header is placed before the result, 
which enables reference counting. If klass is NULL,
a default class will be used that does nothing if done is called
and that uses mem_free to free the memory. If pool is NULL, 
then this works just like obj_alloc. */
void * obj_alloc_in_pool(ObjPool* pool, size_t size, ObjClass * klass) {
  char * ptr                = mem_alloc(size + sizeof(ObjHeader));
  ObjHeader  * aid          = (ObjHeader *) ptr;
  if(!aid) return NULL;
  aid->klass                = klass;
  aid->refcount             = 1;
  // Register the pool if we have one.
  if (pool) {
    objpool_register_data(pool, aid);
  }
  return                    ptr + sizeof(ObjHeader);
}


/** Allocates an object with the given size. 
This will work much like mam_alloc / calloc, except that 
a hidden header is placed before the result, 
which enables reference counting. If klass is NULL,
a default class will be used that does nothing if done is called
and that uses mem_free to free the memory. */
void * obj_alloc(size_t size, ObjClass * klass) {
  return obj_alloc_in_pool(NULL, size, klass); 
}

/* Gets the hidden header for this pointer. Only works on pointers allocated 
with obj_alloc. */
static ObjHeader * obj_objheader(void * ptr) {
  ObjHeader * header;
  char * aid;
  if(!ptr) return NULL;
  aid     = (char *) ptr;
  header  = (ObjHeader *) (aid - sizeof(ObjHeader));
  return header;
}

/** This increases the reference count and returns 
the pointer passed in. Only works on pointer allocated 
with obj_alloc. Use this when you want to retain a reference 
to the object pointed to by ptr.*/
void * obj_ref(void * ptr) {
  ObjHeader * header;
  header = obj_objheader(ptr);
  if(!header) return NULL;
  header->refcount++;
  return ptr;  
}


/** This gets the class info for this object. 
May return null for unclassed objects. Only works on
objects allocated (directly or indirectly) through obj_alloc. */
ObjClass * obj_class(void * ptr) {
  ObjHeader * header = obj_objheader(ptr);
  if (!header) return NULL;
  return header->klass;
}

/** Looks up a function pointer in the class at the given offset. */
ObjMethod * objclass_method_at(ObjClass * klass, size_t offset) { 
  char * aid;
  if(!klass) return NULL;
  aid = (char *) klass;
  return (ObjMethod*) (aid + offset); 
}


/** Looks up the free method for the given class. */
ObjMethod * objclass_getfree(ObjClass * klass) {
  return objclass_method_at(klass, offsetof(ObjClass, free));
}


/** Calls the first step of the destructor (done) on the object
pointed to by ptr. */
void * obj_done(void * ptr) {
  ObjHeader * header;
  ObjClass  * info;
  // look for a destructor in the linked type info tables
  for (info = obj_class(ptr); info; info = info->up) {
    if(info->done) { // call done destructor if found.
      return info->done(ptr);
    }
  }
  // if we get here do nothing.
  return ptr;
}

/** Calls the second step of the destructor (free) on the object that must have
been allocated though obj_alloc and pointed to indirectly by ptr. If the object 
has no class or no free function, mem_free will be called to free the memory 
that ptr(indirectly) references. Give the class a do-nothing free function to 
avoid this if needed. Returns NULL, or whatever the free method
of the object's class returns. The idea is to call this as ptr = obj_free(ptr). 
*/
void * obj_free(void * ptr) {
  ObjHeader * header;
  ObjClass  * info;
  // look for a destructor in the linked type info tables
  for (info = obj_class(ptr); info; info = info->up) {
    if(info->free) { // free destructor if found.
      return info->free(ptr);
    }
  }
  // If we get here, call the default free. First get the header...
  header = obj_objheader(ptr);
  //... then deallocate through the header, since the pointer to the header 
  // points to the top of the memory allocated though obj_alloc
  // if the header is not found, try to call mem_free directly.
  if (header) mem_free(header); else mem_free(ptr);  
  return NULL;
}


/** Reduce the reference count of the object, possibly calling done and free 
on the object if it's reference count became 0. This will return NULL if 
the reference count became 0, otherwise it will return ptr. */
void * obj_unref(void * ptr) { 
  ObjHeader * header;
  header = obj_objheader(ptr);
  if(!header) return NULL;
  header->refcount--;
  /* only free at exactly 0, if unref is called too much, 
    it will go negative and the free will not be called too many times */
  if(header->refcount == 0) {
    // first call the destructor, then the free function.
    obj_done(ptr);
    obj_free(ptr); 
    return NULL;
  }
  return ptr;
}


/** Walks though the object pool, and calls unref on each object. 
If the object is effectively destroyed, it will be removed
from the pool too. Otherwise, it remains in the pool. */
ObjPool * objpool_unref(ObjPool * pool) {
  ObjPoolNode * node, * next, * prev;
  if(!pool) return NULL;
  prev = NULL;
  node = pool->last; 
  while (node) {
    if(!(obj_unref(node->data))) { 
    // not correct, need to change ObjPoolNode!!!
      next = node->next;
      if (!prev) { // begin of list
        pool->last = node->next;
      } else {
        prev->next = next;
      }
      mem_free(node);
      node = next;
    }
  }
  
}

