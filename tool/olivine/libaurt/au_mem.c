/**
* Memory management and garbage collection for the OI programming 
* language runtime.
*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "au_mem.h"

/* Roll my own reference counting. */
  
/* Calloc wrapper with some extra security, 
   such as allocating at least 1 byte if sucessful.
*/

void * au_malloc(au_size_t size) {
  if (size < 1) size = 1;
  return calloc(size, 1);
}

/* Simple free wrappern, that may be chained. Returns NULL (guaranteed) */
void * au_free(void * ptr) {
  free(ptr);
  return NULL;
}

/* 
  Realloc wrapper that does it's best not to overwrite the original pointer
  if it failed. Returns non-null on sucess (new pointer allocated), null on 
  failiure.
*/
void * au_realloc(void ** ptr, au_size_t size) {
  void * newptr;
  if (size < 1) { size  = 1;   }
  newptr                = realloc(*ptr, size);
  if (!newptr)  { return NULL; }
  (*ptr)                = newptr;  
  return newptr;
}


void * au_mem_free_members(void * ptr) {
  au_mem * self       = (au_mem *) ptr;
  if(!self)           { return NULL; }
  self->ref--; 
  if(self->ref > 0)   { return NULL; }
  if(!self->free)     { return NULL; } 
  self->free(self->ptr);
  self->ptr           = NULL;
  return self;  
}

void * au_mem_free_self(void * ptr) {
  au_mem * self       = (au_mem *) ptr;
  if(!self)           { return NULL; }
  return au_free(self);
} 

void * au_mem_free_all(void * ptr) {
  return au_mem_free_self(au_mem_free_members(ptr));
}

/* Frees a block of memory. Calls the destructor with self cast to void * 
   as an argument if the destructor is set. */
au_mem * au_mem_free(au_mem * self) {
  if(!self)           { return NULL; }
  if(!self->free)     { return NULL; }
  return (au_mem *) self->free((void *) self);  
}

/* Returns a reference to the memory block, and increases it's reference count. */
au_mem * au_mem_ref(au_mem * self) {
  if(self) { self->ref++; }
  return self;
}

/* 
  Wraps a pointer in the given au_mem. 
  If the free func is null, it will be set to au_mem_free_members.   
*/
au_mem * au_mem_wrap(au_mem * self, void * ptr) { 
  if(!self)   { return NULL; } 
  self->ptr  = ptr;
  self->ref  = 1;
  return self;  
}

/* Sets the destructor for the memory block. */
au_mem * au_mem_free_set(au_mem * self, au_free_func freef) {
  if(!self)   { return NULL; }
  self->free = freef;
  return self;
}

/* Allocates memory into the block self. Return null if failiure, and self on success. */
au_mem * au_mem_malloc(au_mem * self, au_size_t size) {
  void * ptr; 
  if(!self)   { return NULL; }
  ptr         = au_malloc(size);
  if(!ptr)    { return NULL; } 
  return au_mem_wrap(self, ptr); 
}

/* Rellocates memory into the block self. Return null if failiure, and self on success. */
au_mem * au_mem_realloc(au_mem * self, au_size_t newsize) {
  void * newptr;
  if(!self)   { return NULL; }
  newptr    = au_realloc(self->ptr, newsize);
  if(!newptr) { return NULL; }    
  self->ptr = newptr;
  return self;
}

/* Allocates a new, empty memory block. */
au_mem * au_mem_alloc() {
  au_mem * self;
  result      = au_malloc(sizeof(* self));
  return result; 
}
/* Allocates a memory block, and wraps a pointer in it. */
au_mem * au_mem_new(void * pointer) {
  
}






void au_mem_tonil(au_mem **self) {
  au_mem_free(*self);
  (*self) = NULL;
}

void au_unref(au_mem * mem) {
  au_mem_free(mem);
}


/* Stack-based handlers. */

au_mem au_mem__wrap(void * ptr, au_free_func freefunc) {
  au_mem self;
  au_mem_init(&self, ptr, freefunc); 
  return self;  
}

au_mem au_mem__new(au_size_t size) {
  au_mem self; 
  au_mem_salloc(&self, size);
  return self;
}

au_mem au_mem__realloc(au_mem self, au_size_t newsize) {
  au_mem_realloc(&self, newsize);
  /**XXX: Need some error checking here. **/
  return self;
}

au_mem au_mem__ref(au_mem mem) {
  au_mem_ref(&mem);
  return mem; /* Automatically copies the data. */
}

void au_mem__free (au_mem mem) {
  au_mem_free(&mem);
} 

void * au_obj_ll(au_obj * self) {
  if(!self) { return NULL; }
  return self->memory.ptr;
}

typedef au_obj * (*au_imp)(au_obj * obj, ...);
  
au_obj * myclass_foobar(au_obj * self, char * other, char c, int try) {
  /* 
  va_list args;
  au_obj * other;
  
  va_start(args, self); 
  other = va_arg(args, au_obj *) ;  
  va_end(args);
  */

  printf("%s\n", (char *)au_obj_ll(self));
  printf("%s, %c, %d\n", (char *)other, c, try);
  return self;
}
  
au_imp sel_foobar = (au_imp) &myclass_foobar;  

struct au_sel_ ; 
typedef struct au_sel_ au_sel;

struct au_sel_ {
  char * name;
  size_t size;
  size_t hash;
};


int au_sel_equal_p(au_sel * sel1, au_sel * sel2) {
  if(sel1->size != sel2->size) { return FALSE; }
  if(sel1->hash != sel2->hash) { return FALSE; }
  return (strcmp(sel1->name, sel2->name) == 0);
}



#define au_MEM_PTR(self)          ( (self)->ptr )
#define au_MEM_SPTR(self)         ( (self).ptr )
#define au_MEM_PGET(self, type, index)  ( ((type)au_MEM_PTR(self))+(index) )
#define au_MEM_AGET(self, type, index)  ( ((type)au_MEM_PTR(self))[(index)] )
#define au_MEM_SAGET(self, type, index) ( ((type)au_MEM_SPTR(self))[(index)] )

/* Dictiornary for looking up implementations from selectors.
 * Based on a 2**n sized open hash.   
 */
struct au_impdict_ ;  
struct au_impslot_ ;

typedef struct au_impdict_ au_impdict;
typedef struct au_impslot_ au_impslot;




struct au_impslot_ {
  au_imp method;
  au_sel selector;
};


struct au_impdict_ {
  au_mem slots;
  size_t  size;
  size_t space;  
};

#define au_IMPDICT_INITSPACE 64


au_impdict * au_impdict_init(au_impdict * self) {
  size_t index;
  self->size      = 0;
  self->space     = au_IMPDICT_INITSPACE;   
  au_mem_salloc(&self->slots, sizeof(au_impslot) * self->space);
  for(index       = 0; index < self->space; index ++) {
    au_impslot * slot;
    slot          = ((au_impslot *) self->slots.ptr) + index;
    slot->method  = NULL;
    slot->selector=  (au_sel) { NULL, 0, 0 };
  }  
   
  /*  au_MEM_SAGET(self->slots, au_impdict *, index).method    = NULL;
    au_MEM_SAGET(self->slots, au_impdict *, index).selector  = NULL;
    */
  
  return self;
}

void au_impdict_free(au_impdict * self) {
  
}

au_impdict * au_impdict_new() {
  au_impdict * self;
  self        = (au_impdict *) au_mem_new(sizeof(au_impdict));
  if(!self)   { return NULL; }
  return au_impdict_init(self);
}

au_impdict * au_impdict_add(au_impdict * self, au_sel selector, au_imp method) {
  

  return self;
}  



#ifdef au_ALLOC_TEST
#include <stdio.h>

void use_mem(au_mem mem) {
  mem = au_mem__ref(mem);
  printf("ptr2 %p, %ld\n", mem.ptr, mem.ref);
  au_mem__free(mem);
}

int main(void) {
  au_impdict  dict;
  au_obj obj, obj2;
  au_mem mem, mem2, mems;
  au_impdict_init(&dict);
  
  obj.memory = au_mem__wrap("Hello there 1!", NULL);
  obj2.memory = au_mem__wrap("Hello there 2!", NULL);
  sel_foobar(&obj, "OK!", 'c', 123);
  
  printf("ptr1 %p %u\n", mem.ptr, sizeof(au_obj));
  mems  = au_mem__wrap("Hello, static memory!", NULL);
  mem   = au_mem__new(123);
  mem2  = au_mem__ref(mem);   
  printf("ptr2 %p, %ld\n", mem2.ptr, mem2.ref);
  /* ptr2  = au_ref(ptr); */
  use_mem(mem);
  use_mem(mems);  
  /*
  au_mem__free(mem);
  au_mem__free(mem2);
  au_mem__free(mems);
  au_mem__free(obj.memory);
   au_mem_free(&dict.slots);
   au_mem_free(&dict);
   au_free(ptr2); */
  return 1;

}
#endif
