#include "dynar.h"
#include "mem.h"

#define ARRAY_ROOM 16

/**
* Dynar is a DYNamic ARray of size elements each of elsz size.
* For simplicity, dynarr is exactly sized, that is,
* it does not allocate any extra elements but the amount requested.
* In other words it's capacity is equal to it's size.
*/
struct Dynar_ {
  char * data;
  // use a char * pointer since that makes pointer arithmetic easier
  int size;
  int elsz;
};

/** Gets the array's size. Returns 0 if self is NULL. */
int dynar_size(Dynar * self) {
  if(!self) return 0;
  return self->size;
}

/** Gets the amount of elements in the vector. Returns 0 if self is NULL. */
int dynar_amount(Dynar * self) {
  if(!self) return 0;
  return self->size;
}

/** Gets the array's element size.  Returns 0 if self is NULL. */
int dynar_elementsize(Dynar * self) {
  if(!self) return 0;
  return self->elsz;
}

/** Frees the contents of an array. Has the same effect as emptying the array. Does not call a desctructor on any elements contained! */
Dynar * dynar_done(Dynar * self) {
  if(!self) return NULL;
  mem_free(self->data);
  self->data = NULL;
  self->size = 0;
  return self;
}

/** Calls a destructor on the contents of the array if it is not null. 
 The contents are considered to be pointers. */
Dynar * dynar_destroy(Dynar * self, MemDestructor * destroy) {
  int index;
  int size = dynar_size(self);
  if(!self)     return self;
  if(!destroy)  return NULL;
  for(index = 0; index < size ; index++) {
    void * ptr = dynar_getptr(self, index);    
    destroy(ptr);
  }
  return self;
}

/** Frees an array. Returns NULL. */
Dynar * dynar_free(Dynar * self) {
  dynar_done(self);
  return mem_free(self);
}

/** Calls a destructor on the elements of the array and then frees the array */
Dynar * dynar_free_destroy(Dynar * self, MemDestructor * destroy) {
  dynar_destroy(self, destroy);
  return dynar_free(self);
}


/** Allocates a new empty array.  */
Dynar * dynar_alloc() {
  return STRUCT_ALLOC(Dynar);
}  


/** Initializes an empty array with 0 elements of size elsz each. */
Dynar * dynar_initempty(Dynar * self, int elsz) {
  if(!self)    return NULL;
  if(elsz < 0) return NULL;
  self->size = 0;
  self->elsz = elsz;
  return self;
}

/** Allocates a new empty array for elements of size elsz each. */
Dynar * dynar_newempty(int elsz) {
  Dynar * res = dynar_alloc();
  if(!dynar_initempty(res, elsz)) {
    return dynar_free(res);
  }
  return res;
}

/** Changes the size of the dynamic array. Newsize must be >= 1. */
Dynar * dynar_size_(Dynar* self, int newsize) {
  int elsz = dynar_elementsize(self);
  void * newd = NULL;
  if(!self) return NULL;
  // don't allow a newsize of 0, since that will make realloc call
  // free on self->data, which we don't want.
  if(newsize < 1) return NULL;
  newd = mem_realloc(self->data, newsize * elsz);
  if(!newd) return NULL;
  // if we get here realloc was successful, so it should be safe to reassign
  // self->data
  self->data = newd;
  self->size = newsize;  
  return self;
}

/** Initializes a new array with a amount elements of size
elsz each. */
Dynar * dynar_init(Dynar * self, int amount, int elsz) {
  Dynar * aid = dynar_initempty(self, elsz);
  if(!aid) return NULL;
  if(!dynar_size_(self, amount)) return NULL;
  return self;
}

/** Initializes a new array with a capacity to store amount void* pointers.*/
Dynar * dynar_initptr(Dynar * self, int amount) {
  return dynar_init(self, amount, sizeof(void *));
}

/** Allocates a new array with amount elements of size elsz each. */
Dynar * dynar_new(int amount, int elsz) {
  Dynar * res = dynar_alloc();
  if(!dynar_init(res, amount, elsz)) {
      return dynar_free(res);
  }
  return res;
}


/** Allocates a new array that will be able to contain amount void * pointers.*/
Dynar * dynar_newptr(int amount) {
  return dynar_new(amount, sizeof(void *));
}


/** Sets the amount of elements of the the array, but ony if
* amount bigger than the bigger than the current size.
* Returns NULL if the array was not grown, otherwise returns self.
*/
Dynar * dynar_grow(Dynar * self, int amount) {
  int mysize = dynar_size(self);
  if (mysize >= amount) return NULL;
  return dynar_size_(self, amount);
}

/** Checks if the index is smaller than the array's available room . */
int dynar_sizeindex_ok(Dynar * self, int index) {
  if(!self) return FALSE;
  return index < dynar_size(self);
}

/** Checks if the int index is smaller than the array's available room. */
int dynar_index_ok(Dynar * self, int index) {
  if(!self) return FALSE;
  if (index < 0) return FALSE;
  return dynar_sizeindex_ok(self, (int)(index));
}

/** Returns a pointer to the index-th element of the array.
Does no bounds checking! */
void * dynar_getraw_unsafe(Dynar * self, int index) {
  return self->data + (index * self->elsz);
}

/** Copies dynar_elementsize(self) of bytes from the index-th element
of the array to out, which must be pointing to a bufer of at least
dynar_elementsize(self). Does no bounds checking!
Returns NULL on failure, out on success.
*/
void * dynar_getcopy_unsafe(Dynar * self, int index, void * out) {
  char * cptr = (char *) dynar_getraw_unsafe(self, index);
  int size = dynar_elementsize(self);
  if((!self) || (!out) || (!cptr)) return NULL;
  mem_move(out, cptr, size);
  return out;
}

/** Copies dynar_elementsize(self) of bytes from the data pointed to
* by ptr into the location pointed to by index.
* Does no bounds checking!
*/
Dynar * dynar_putraw_unsafe(Dynar * self, int index, void * ptr) {
  char * cptr = (char *) dynar_getraw_unsafe(self, index);
  int size = dynar_elementsize(self);
  if((!self) || (!ptr) || (!cptr)) return NULL;
  mem_move(cptr, ptr, size);
  return self;
}

/** Returns a pointer to the index-th element of the array.
Does bounds checking and return NULL if out of bounds */
void * dynar_getraw(Dynar * self, int index) {
  // Bounds check
  if(!dynar_index_ok(self, index)) { return NULL; }
  return dynar_getraw_unsafe(self, index);
}

/** Returns a pointer to the index-th element of the array.
Does bounds checking and return NULL if out of bounds */
void * dynar_getcopy(Dynar * self, int index, void * ptr) {
  // Bounds check
  if(!dynar_index_ok(self, index)) { return NULL; }
  return dynar_getcopy_unsafe(self, index, ptr);
}

/** Copies the dynar_elementsize(self) of bytes from the data pointed to
* by ptr into the location pointed to by index.
* Does bounds checking and return NULL if ouut of bounds.
*/
Dynar * dynar_putraw(Dynar * self, int index, void * ptr) {
  // Bounds check
  if(!dynar_index_ok(self, index)) { return NULL; }
  return dynar_putraw_unsafe(self, index, ptr);
}

/** Stores a pointer at the index of the array.
* Does bounds checking. dynar_elementsize(self) sould have been
* initialized as sizeof(void *) by using dynar_newptr
*/
void * dynar_putptr(Dynar * self, int index, void * ptr) {
  return dynar_putraw(self, index, &ptr);
  // use &ptr because we want to put the contents of the pointer,
  // not the pointer itself. 
}

/** Returns a pointer that was stored at the index index of the array. */
void * dynar_getptr(Dynar * self, int index) {
  void * res = NULL; 
  if(!dynar_getcopy(self, index, &res)) return NULL;
  // use &ptr because we want to fetch the contents of the pointer,
  // which will be copied from the element of the array.
  return res;
}

/** Copies the element that *ptr points to into this array at position
index */
void * dynar_putdata(Dynar * self, int index, void * ptr) {
  return dynar_putraw(self, index, ptr);
}

/** Returns a pointer to the index-th element of the array. */
void * dynar_getdata(Dynar * self, int index) {
  return dynar_getraw(self, index);
}

/* Applies quick sort to the array using the given comparator. */
Dynar * dynar_qsort(Dynar * self, DynarCompare  * compare) {
  void * base; int nmemb; size_t size;
  if(!self) return NULL;
  base  = self->data;
  nmemb = self->size;
  size  = self->elsz;
  qsort(base, nmemb, size, compare);
  return self;
}

/* Applies a binary search to the array using the given comparator. 
 User must call dynar_qsort first. */
void * dynar_bsearch(Dynar * self, const void * key, DynarCompare  * compare) {
  void * base; int nmemb; size_t size;
  if (!self) return NULL;
  base  = self->data;
  nmemb = self->size;
  size  = self->elsz;
  return bsearch(key, base, nmemb, size, compare);
}


/* Iterator helper: fill in every->now as data. */
Every * dynar_everynow_data(Every * every) {
  every->now   = dynar_getdata(every->on, every->index);
  if(every->now) return every;
  return NULL;
}

/* Iterator helper: fill in every->now as pointer. */
Every * dynar_everynow_ptr(Every * every) {
  every->now   = dynar_getptr(every->on, every->index);
  if(every->now) return every;
  return NULL;
}

/* Iterator helpers: init */
Every  * dynar_everyinit_data(Every * every) {
  every->index = 0;
  return dynar_everynow_data(every);
}

/* Iterator helpers: next */
Every  * dynar_everynext_data(Every * every) {
  every->index++;
  return dynar_everynow_data(every);
}

/* Iterator helpers: put. */
void  * dynar_everyput_data(Every * every, void * data) {
  return dynar_putdata(every->on, every->index, data);
}

/* Iterator helpers: done. */
void  * dynar_everydone(Every * every) {
  return every;
}

/* Iterator helpers: init pointers */
Every  * dynar_everyinit_ptr(Every * every) {
  every->index = 0;
  return dynar_everynow_ptr(every);
}

/* Iterator helpers: next pointers */
Every  * dynar_everynext_ptr(Every * every) {
  every->index++;
  return dynar_everynow_ptr(every);
}

/* Iterator helpers: put pointers. */
void  * dynar_everyput_ptr(Every * every, void * data) {
  return dynar_putptr(every->on, every->index, data);
}


/* Iterator helper table. */
static EveryActs dynar_every_data_acts_ = {
  dynar_everydone,
  dynar_everyinit_data,
  dynar_everynext_data,
  dynar_everyput_data  
};

/* Iterator helper table. */
static EveryActs dynar_every_ptr_acts_ = {
  dynar_everydone,
  dynar_everyinit_ptr,
  dynar_everynext_ptr,
  dynar_everyput_ptr
};


/** Iterates over the data. Call every_free when done. */
Every * dynar_every_data(Dynar * dynar) {
  return every_new(&dynar_every_data_acts_);
}

/** Iterates over the pointers in this array. Call every_free when done. */
Every * dynar_every_ptr(Dynar * dynar) {
  return every_new(&dynar_every_ptr_acts_);
}


/* Walks over the array using the each interface, accessing
the data as pointers. eachdo should return non-null to break the iteration,
the data thus found is returned bu this function. */
void * dynar_each_ptr(Dynar * self, EachDo * eachdo, void * extra) {
  Each each;
  int index;
  int size = dynar_size(self);  
  each_init(&each, self, extra); 
  for(index = 0; index < size ; index++) {
    void * aid;
    void * ptr = dynar_getptr(self, index);    
    each_next(&each, ptr);
    aid = eachdo(&each);
    if (aid) return aid;
  }
  return NULL;
}

/* Walks over the array using the walker interface, accessing
the data as stored structs. */
void * dynar_walkdata(Dynar * self, EachDo * eachdo, void * extra) {
  Each each;
  int index;
  int size = dynar_size(self);
  each_init(&each, self, extra);
  for(index = 0; index < size ; index++) {
    void * aid;
    void * ptr = dynar_getdata(self, index);
    each_next(&each, ptr);
    aid = eachdo(&each);
    if (aid) return aid;
  }
  return NULL;
}


/**
* Lilis is a doubly Linked List that points to it's members via void pointers 
* but does not own them  in the sense that it doesn't clean them up unless requested.
*/
struct Lilis_ {
  Lilis * next;
  Lilis * prev;
  void  * data;
};


/**
* frees the tail of a llist only. Does nothing to self or other.
*/

Lilis * lilis_freetail(Lilis * self) {
  Lilis * next, * now;
  if(!self) return NULL;
  now = self->next;   // skip current node, that will be deleted in lilis_free
  while (now) { 
    next = now->next; // already get next one
    mem_free(now);    // now we can safely free the current node
    now  = next;      // set current to next one.
  }
  return self;
}


/** Frees the tail of the list pointed to by self. It does not free any of the 
emlements contained in * data. Does not alter self->prev, and doesn't 
free that either. */
Lilis * lilis_done(Lilis * self ) {  
  if(!self) return NULL;
  lilis_freetail(self);
  self->data = NULL;
  self->next = NULL;
  return self;
}

/** Frees a linked list node and it's tail. Returns NULL. */
Lilis * lilis_free(Lilis * self) {
  lilis_done(self);
  return mem_free(self);
}

/** Allocates a new empty linked list.  */
Lilis * lilis_alloc() {
  return STRUCT_ALLOC(Lilis);
}  


/** Initializes a linked list node */
Lilis * lilis_init(Lilis * self, Lilis * next, Lilis * prev, void * data) {
  if(!self)    return NULL;  
  self->data = data;
  self->next = next;
  self->prev = prev;
  return self;
}

/** Initializes a new linked list node that points to nothing. */
Lilis * lilis_initempty(Lilis * self) {
  return lilis_init(self, NULL, NULL, NULL);
}


/** Returns a newly allocatd linked list node. */
Lilis * lilis_new(Lilis * next, Lilis * prev, void * data) {
  Lilis * res = lilis_alloc();
  if(!lilis_init(res, next, prev, data)) {
    return lilis_free(res);
  }
  return res;
}

/** Returns a newly allocated linked list node that points to nothing. */
Lilis * lilis_newempty() {
  return lilis_new(NULL, NULL, NULL);
}

/** Appends a node to the current one, inserting it if needed.
* returns other.
*/
Lilis * lilis_add(Lilis * self, Lilis * other)  {
  Lilis * oldnext = self->next;
  if ((!self) || (!other)) return NULL;
  if (oldnext) {
    oldnext->prev = other;   // next one's previous becomes the inserted link    
    other->next   = oldnext; // inserted link's next is the old next
  }
  // these two belowear always needed even if there is no oldnext. 
  other->prev   = self;    // and other's previous one is self.
  self->next    = other;   // and curent next is the inserted link
  return other;
}

/**  Creates a new node and adds it */
Lilis * lilis_addnew(Lilis * self, void * data) {
  Lilis * other = lilis_new(NULL, NULL, data);
  return lilis_add(self, other);
}


/** Removes the node that follows self, but does NOT call lilis_free on it. 
* returns the removed node
*/
Lilis * lilis_removenext(Lilis * self) {
  Lilis * next, * nextnext;
  if(!self) return NULL;
  next = self->next;
  if(!next) return NULL;
  nextnext = next->next;
  if(nextnext) {
    nextnext->prev = self;
    self->next     = nextnext;
  } else {
    self->next     = NULL;
  }
  return next;
}

/** Removes self, modifies prev if this is needed, but does NOT call 
* lilis_free on it. 
* returns the removed node
*/
Lilis * lilis_remove(Lilis * self) {
  Lilis * prev, * next;
  if(!self) return NULL;
  next = self->next;
  prev = self->prev;
  if(next) { // link prev node to next node, so this node is cut out. 
    prev->next = next;
    next->prev = prev;
  } else { // no next node, prev also gets no next node.
    prev->next = NULL;
  }
  // finally unlink self.
  self->next = NULL;
  self->prev = NULL;
  return next;

}

/** Removes the node self, and calls lilis_free on it.
* returns NULL
*/
Lilis * lilis_erase(Lilis * self) {
  Lilis * eraseme = lilis_remove(self);
  return lilis_free(eraseme);
}



/** Removes the node that follows self, and calls lilis_free on it.
* returns NULL
*/
Lilis * lilis_erasenext(Lilis * self) {
  Lilis * eraseme = lilis_removenext(self);
  return lilis_free(eraseme);
}

/** Gets the next node in the list. */
Lilis * lilis_next(Lilis * self) {
  if(!self) return NULL;
  return self->next;
}

/** Gets the previous  node in the list. */
Lilis * lilis_previous(Lilis * self) {
  if(!self) return NULL;
  return self->prev;
}

/** Gets the data of the list node */
void * lilis_data(Lilis * self) {
  if(!self) return NULL;
  return self->data;
}
