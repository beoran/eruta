
#define NORI_INTERN_ONLY
#include "nori_intern.h"



struct NoriClass_;
typedef struct NoriClass_ NoriClass;

struct NoriObject_; 
typedef struct NoriObject_ NoriObject;

NORI_FUNCTYPE(NoriObject *) NoriMethod(NoriObject * args);

struct NoriVtable_;
typedef struct NoriVtable_;
struct NoriVtable_ {
  NoriMethod **methods;
  NoriSize     size;
};
 
struct NoriClass_ {
  NoriCurator * curator;
  NoriVtable    table;
};

struct NoriObject_ {
  NoriClass  * klass;
  NoriObject * own;
  NoriSize     refs;
};


NoriObject * nori_object_init(NoriObject * obj, NoriClass * klass, 
                              NoriObject * own) {
  obj->klass = klass;
  obj->own   = own;
  obj->refs  = 1;  
} 



/** 
* A rod is an immutable text, otherwise known as "immutable string"
* Encoded in utf-8 or ascii. 
*/

struct NoriRod_;
typedef struct NoriRod_ NoriRod;

NoriClass NoriStaticRodClass; 
NoriClass NoriRodClass;
NoriClass * NoriStaticRodClassPtr = NULL; 
NoriClass * NoriRodClassPtr       = NULL; 

struct NoriRod_ {
  char            * str;  
  NoriSize          size;
  int               mode; // details on memory allocation. 
};

/*
 Not only for strings, but for all objects in the memory which are held 
 inside a container(owner), there are several ways for the container to take
 ownership of the containee:
   
 1) The contained pointer points to a statically allocated location and 
    is in essence immutable, such as a pointer to a string constant or to an integer constant. In this case, the container is not really the owner, the program itself is. The container should not and must not free the memory pointed to. The reference is in essence "borrowed", but normally, the program cannot deallocate or mess up this memory either, so it should always be safe to use such a pointer. I'll call this a rented reference. 
    
 2) The pointer points to a global variable, or a static variable in a C file.      In this case, the program is also the owner, and responsable for setting up     and releasing any memory allocated to the global variable, although it could    delegate this reponsability to, say, some functions in modules. 
    The reference is in essence "borrowed". Although the pointer cannot become invalidated, the date paointed to can, so it's a still risky to use global variables, I guess.
    
 3) The pointer points to (temporary) memory on the stack. Generally, if 
    the container is supposed to persist, then the data pointed to should be copied to a newly allocated memory area whhich the container will own, 
    and which must then be freed when the container is disposed of. But if 
    the container is temporary, that is, shorter lived than the containee, it might be enough to borrow a reference to it.
    
 4) the pointer points to malloced() memory. Generally, a copy should be made.      However, if the malloced memory location has no other owner, a copy might       not be needed, especially if it is to wrap a pointer that contains memory       allocated by some C library function. Anyway, in both cases, 
    the container has to free the memory. Or, when it's a weak reference, 
    a weak reference object can be allocated to hold the borrowed reference. 
    
  So basically there are two questions: copy or not copy, and free or not free.
  
  No copy, no free: Borrowed reference. 
  No copy, do free: Took ownership of reference.
  Do copy, no free: Error, causes memory leaks.
  Do copy, do free: Took ownership and copied otherwise data that was volatile
                    or owned by other container.
   
  
  Pointer To : | Constant   | Global    | Stack         | Malloced  
  -------------+------------+-----------+---------------+-----------
  Copy ?       | No         | No        | Yes, usually  | Yes, usually
  Free ?       | No         | No        | Yes           | Yes, anyway.
   
  
  => An owner is the container that is "responsible" for cleaning up the
  malloced data that it has pointers to. It does this by calling the
  needed free functions on the object when it gets freed itself.
   
  What do we need custom functions for? When dealing with memory allocation, there are several things we can do: allocate, resize, deep copy, shallow 
  copy  and free. It seems it depends on how to deal with these whether or 
  not we need such custom functions.
  

*/

#define NORI_ROD_CONST
#define NORI_ROD_GRAB
#define NORI_ROD_COPY

NoriRod * nori_rod_init(NoriRod * rod, char * str, int mode) {
  rod->size     = strlen(str);
  rod->constant = constant;
  if(constant || grab) {
    rod->str  = str;
  } else {
    rod->str  = nori_alloc(rod->size);
    if(!rod->str) { return NULL; } 
    strncpy(rod->strn, str, rod->size);
  }
  return rod;
}


NoriRod * nori_rod_done(NoriRod * rod) {
  if(!rod) return NULL;
  if(!rod->constant) { 
    nori_free(rod->str); rod->str = NULL;
  }
  return rod;
}

void nori_rod_free(NoriRod * rod) {
  nori_rod_done(rod);
  nori_free(rod);
}

NoriRod * nori_rod_new(char * str, int constant, int grab) {
  NoriRod * rod = nori_alloc(sizeof(NoriRod));
  if(!rod) return NULL;
  return nori_rod_init(rod, str, constant, grab);
}   

NoriRod * nori_rod_grab(char * str) {
  return nori_rod_new(str, FALSE, TRUE);
}   

NoriRod * nori_rod_copy(char * str) {
  return nori_rod_new(str, TRUE, FALSE);
}   

NoriRod * nori_rod_grab(char * str) {
  return nori_rod_new(str, TRUE);
}   



