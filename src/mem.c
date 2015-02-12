/* Memory handling, malloc wrapers, etc */

#include <stdlib.h>
#include <string.h>
#include "mem.h"


/* Calls exit(EXIT_FAILURE) if ptr is NULL. Is messqhe is not null, prints this on stderr.  */
void * exit_if_null(void * ptr, char * message) {
  if(!ptr) {
    if(message) {
      fprintf(stderr, "%s\n", message);
    }
    exit(EXIT_FAILURE);
  }
  return ptr;
}

#define EXIT_ON_NOMEMORY(PTR) exit_if_null(PTR, "Out of memory!")

/** Wrapper for calloc/malloc */
void * mem_calloc(size_t amount, size_t size) {
  void * ptr = calloc(amount, size);
  return EXIT_ON_NOMEMORY(ptr);
}

/** Wrapper for calloc/malloc */
void * mem_alloc(size_t size) {
  void * ptr = mem_calloc(1, size);
  return EXIT_ON_NOMEMORY(ptr);
}

/** Wrapper for free */
void * mem_free(void * ptr) {
  free(ptr);
  return NULL;
}

/** Wrapper for realloc */
void * mem_realloc(void *ptr, size_t newsize) {
  void * aid = realloc(ptr, newsize);
  return EXIT_ON_NOMEMORY(aid);
}

/** Resizes memory, taking care not to lose ptr* if the reallocation
fails. */
void * mem_resize(void ** ptr, size_t newsize) {
  if(*ptr) { 
    void * res = mem_realloc((*ptr), newsize);
    if(res) {
      (*ptr) = res;
      return res;
    }
  }  
  return NULL;
}

/** Wrapper for memmove, for consistency */
void * mem_move(void * dest, void * src, size_t size) {
  return memmove(dest , src, size);
}

/* Thoughts on hierarchical data structures.
* Often, in C data is allocatedin a hierarchical way. 
* It's useful to have functions that take this into consideration...
*/ 

typedef struct Memtree_ Memtree;

struct Memtree_ {
  void * memory;
  size_t size;
  Memtree * next, * parent, * child;
};

#ifdef COMMENT_
/* Tree memory allocator. */
void * mem_tree_alloc(void * vparent, size_t size) {
  char * aid;
  Memtree * me, * next;
  Memtree * parent = vparent;
  aid        = mem_calloc(1, size + sizeof(Memtree));
  if (!aid) return NULL;
  me         = (Memtree *) aid;
  me->memory = aid + sizeof(Memtree); 
  me->size   = size;
  if (parent) {
    me->parent = parent;
    if (parent->child) {
      next = parent->child;
    } else {
      next = NULL
    }
        
    parent->child = me;
    me->next      = next;
  } else {
    me->parent   = NULL;
    me->next     = NULL;
    me->child    = NULL;
  }
  return me->memory;
}


void mem_tree_free(void * vparent) {
  char * aid;
  Memtree * me, * next;
  Memtree * parent = vparent;
  aid        = vparent;
  if (!aid) return;
  me         = (Memtree *) (aid + sizeof(Memtree));
  if (!me) return;
  me->memory = NULL;
  me->size   = 0; 
  if (me->child) {
    mem_tree_free(me->child);
  } else if (me->next) {
    for (next = me->next; next; next = next->next) {
      
      mem_tree_free(next);
    } 
  } else {
    next = NULL
  }
  
  mem_free(me);  
}

#endif

/* 
* Thoughts on memory allocation in C. 
*
* In theory, memory allocations could just use malloc an free and 
* that would be enough, however, in practise there are several factors that
* mean that just calling free() on any pointer won't free all allocated
* memory and resources associated to it. In general, when a pointer to 
* a resource needs to be cleaned up, there are 3 concens to take care of:
*
* 1) Cleaning up of non-memory resources like OS handles, API haandles, 
* FILEs, etc. that must be closed or cleaned up before the pointer/struct 
* that refers to them may be deallocated. In general, this could be 
* called deinitialization.
*
* 2) The second problem is that the pointer may point to a complex data
* structure that may have been allocated my several mallocs. In this case
* multiple frees are needed. This could be called deconstruction.
* 
* 3) Finally the pointer will usually point to allocated memory that must 
* be deallocated. For this, free could suffice, but not in all cases.
* This could be deallocation.
*
* These 3 steps are of course mirrored when the resource is created, as can 
* be seen in the diagram below.
* 
*               step                  state of resource
*
* Before creation:                  : VOID
*  
*           /- allocation           : ALLOCATED
* Creation {-- construction         : CONSTRUCTED
*           \- initialization       : INITIALIZED
*
* Use of the resource....
*
*              /- deinitialization  : CONSTRUCTED
* Destruction {-- deconstruction    : ALLLOCATED
*              \- deallocation      : VOID
*
* It should be noticed that with proper care an object that has not been
* intialized, or that merely has been deinitialized (i.e. in the CONSTRUCTED
* state) could be used or reused simply by initializing or reinitializing it. 
* It could be useful for performance or practical reasons or to enable such
* recycling of already allocated and constructed resources. 
* 
* On the other hand a resource that has been deallocated or not allocated 
* cannnot be used correctly at all. What then about a resource that has not 
* been constructed yet or one that has already been deconstructed? The barrier
* is more fluid and uncrear here. However, in many progaming languages, these
* 3 steps get simplified ito 2 or even 1 step. However, experience shows that 
* having only one step is too gross, especially in the face of reference 
* counting or on-GC deinitialization. Therefore, it's best to have 2
* steps on construction and 2 steps on deconstruction namely 
*
* construction by XXX_new  : XXX_build    XXX_init. 
* destruction  by XXX_free : XXX_done     XXX_demolish
*
* The rule is that XXX_done and XXX_init should be complementary, 
* any resource RES that has XXX_init(XXX_done(RES), params...) called 
* will be usable again. Furthemore XXX_done should be idempotent,
* that is XXX_done(XXX_done(RES)), etc is allowed. 
* XXX_init need not be idempotent as long as it can undo XXX_done correctly.
* XXX_free is guaranteed to call XXX_done and XXX_demolish. 
* XXX_demolish must not neccesarily be exposed on the public API or 
* even exist.
* XXX_new calls XXX_build and XXX_init in turn.
*
*
* The problem remains then : how to know which destructor to call?
* In case it's known at compile-time  which other destructors will be needed
* it's simple. However, some containers may want to store objects whose type 
* is only known at run time. For them it is neccesary to either not accept 
* ownership and let some other routine deallocate them) which is 
* bothersome, certainy for, say, hash tables, or to properly devise a system
* where the correct destructors can be passed at run time. 
* 
* And then we arrive at interfaces that use and/or consist of virtual tables, 
* that is tablew with function pointers. For the destructor a single pointer
* could suffice, but while we're going virtual, allowing interface/class like
* tables will be better since it alllows more generality and saves space.
*
*
* Another point is what to do if memory allocation fails. While for a critical 
* application, it woule make sense to try to free ups some space and try
* again, for non-critical ones like Eruta, the best thing to do is to 
* simply end the program, since there's nothing sensible that could be done,
* apart from trying to restore the sceen to it's previous resolution if possible.
*  
*
*/



/** Thoughts on memory management and garbage collection. 
* The probelm in C is that it is not always clear who is the owner 
* for an object, and and who is just using a reference. 
* If ownership is well defined, then correct cleanup is easier to arrange.
*
* Here's an idea on reference linking for shared ownership:
*/

typedef struct Memref_ Memref;

struct Memref_ {
  void   * pointer;
  Memref * next;
  Memref * previous;
};

Memref memref(void * pointer) {
  Memref ref = { pointer, 0, 0 };
  return ref;
}











