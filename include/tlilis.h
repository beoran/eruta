

#define T_MAKE_NAME(T, NAME)              T##_##NAME
#define T_MAKE_NAME2(T, U, NAME)          T##_##U##_##NAME
#define T_MAKE_NAME3(T, U, V, NAME)       T##_##U##_##V##_##NAME
#define T_MAKE_NAME4(T, U, V, W, NAME)    T##_##U##_##V##_##W##_##NAME
#define T_MAKE_NAME4(S,T, U, V, W, NAME)  S##_##T##_##U##_##V##_##W##_##NAME

#define TEMPLATE(T, NAME)                 T_MAKE_NAME(T, NAME)
#define TEMPLATE2(T, U, NAME)             T_MAKE_NAME2(T, U, NAME)
#define TEMPLATE3(T, U, V, NAME)          T_MAKE_NAME3(T, U, V, NAME)
#define TEMPLATE4(T, U, V, W, NAME)       T_MAKE_NAME4(T, U, V, W, NAME)
#define TEMPLATE5(S, T, U, V, W, NAME)    T_MAKE_NAME5(S, T, U, V, W, NAME)


#ifndef T 
#error You must define T as the element type to use this template.
#endif


#define TLILIS    TEMPLATE(T, Lilis)
#define TLILIS_   TEMPLATE(T, Lilis_)
#define TFUNC_    TEMPLATE(T, lilis)

#ifndef TEMPLATE_ALLOC
#define TEMPLATE_ALLOC(SIZE) calloc(size, 1)
#endif

#ifndef TEMPLATE_FREE
#define TEMPLATE_FREE(ptr) free(size, 1)
#endif


/**
* TLILIS is a doubly Linked List that points to it's members via T *pointers 
* but does not own them  in the sense that it doesn't clean them up unless requested.
*/
struct TLILIS_ {
  TLILIS * next;
  TLILIS * prev;
  T      * data;
};


/**
* frees the tail of a llist only. Does nothing to self or other.
*/

TLILIS * TFUNC(freetail)(TLILIS * self) {
  TLILIS * next, * now;
  if(!self) return NULL;
  now = self->next;   // skip current node, that will be deleted in TFUNC(free)
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
TLILIS * TFUNC(done)(TLILIS * self ) {  
  if(!self) return NULL;
  TFUNC(freetail)(self);
  self->data = NULL;
  self->next = NULL;
  return self;
}

/** Frees a linked list node and it's tail. Returns NULL. */
TLILIS * TFUNC(free)(TLILIS * self) {
  TFUNC(done)(self);
  return TEMPLATE_FREE(self);
}

/** Allocates a new empty linked list.  */
TLILIS * TFUNC(alloc)() {
  return TEMPLATE_ALLOC(sizeof(TLILIS));
}  


/** Initializes a linked list node */
TLILIS * TFUNC(init)(TLILIS * self, TLILIS * next, TLILIS * prev, T * data) {
  if(!self)    return NULL;  
  self->data = data;
  self->next = next;
  self->prev = prev;
  return self;
}

/** Initializes a new linked list node that points to nothing. */
TLILIS * TFUNC(initempty)(TLILIS * self) {
  return TFUNC(init)(self, NULL, NULL, NULL);
}


/** Returns a newly allocatd linked list node. */
TLILIS * TFUNC(new)(TLILIS * next, TLILIS * prev, T * data) {
  TLILIS * res = TFUNC(alloc)();
  if(!TFUNC(init)(res, next, prev, data)) {
    return TFUNC(free)(res);
  }
  return res;
}

/** Returns a newly allocated linked list node that points to nothing. */
TLILIS * TFUNC(newempty)() {
  return TFUNC(new)(NULL, NULL, NULL);
}

/** Appends a node to the current one, inserting it if needed.
* returns other.
*/
TLILIS * TFUNC(add)(TLILIS * self, TLILIS * other)  {
  TLILIS * oldnext = self->next;
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
TLILIS * TFUNC(addnew)(TLILIS * self, T * data) {
  TLILIS * other = TFUNC(new)(NULL, NULL, data);
  return TFUNC(add)(self, other);
}


/** Removes the node that follows self, but does NOT call TFUNC(free) on it. 
* returns the removed node
*/
TLILIS * TFUNC(removenext)(TLILIS * self) {
  TLILIS * next, * nextnext;
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
* TFUNC(free) on it. 
* returns the removed node
*/
TLILIS * TFUNC(remove)(TLILIS * self) {
  TLILIS * prev, * next;
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

/** Removes the node self, and calls TFUNC(free) on it.
* returns NULL
*/
TLILIS * TFUNC(erase)(TLILIS * self) {
  TLILIS * eraseme = TFUNC(remove)(self);
  return TFUNC(free)(eraseme);
}



/** Removes the node that follows self, and calls TFUNC(free) on it.
* returns NULL
*/
TLILIS * TFUNC(erasenext)(TLILIS * self) {
  TLILIS * eraseme = TFUNC(removenext)(self);
  return TFUNC(free)(eraseme);
}

/** Gets the next node in the list. */
TLILIS * TFUNC(next)(TLILIS * self) {
  if(!self) return NULL;
  return self->next;
}

/** Gets the previous  node in the list. */
TLILIS * TFUNC(previous)(TLILIS * self) {
  if(!self) return NULL;
  return self->prev;
}

/** Gets the data of the list node */
T * TFUNC(data)(TLILIS * self) {
  if(!self) return NULL;
  return self->data;
}

#undef T
#undef TFUNC
#undef TLILIS
#undef TLILIS_
#undef TEMPLATE_ALLOC
#undef TEMPLATE_FREE

