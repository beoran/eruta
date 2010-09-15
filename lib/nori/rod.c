
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
  Copy ?       | No         | No        | Yes, usually  | Yes, usually.
  Free ?       | No         | No        | Yes           | Yes, anyway.
   
  
  => An owner is the container that is "responsible" for cleaning up the
  malloced data that it has pointers to. It does this by calling the
  needed free functions on the object when it gets freed itself.
   
  What do we need custom functions for? When dealing with memory allocation, there are several things we can do: allocate, resize, deep copy, shallow 
  copy  and free. It seems it depends on how to deal with these whether or 
  not we need such custom functions.
  

*/

/* Memory mode */

#define NORI_MEM_DOCOPY   1
#define NORI_MEM_DOFREE   2

/** Three different things tio do with a reference. 
    Either borrow it and don't become the owner, or become the owner, 
    or copy the data and become the owner of the copy. 
*/
#define NORI_MEM_BORROW   0
#define NORI_MEM_OWN      (NORI_MEM_DOFREE)
#define NORI_MEM_COPY     (NORI_MEM_DOCOPY | NORI_MEM_FREE)

#ifndef NORI_ROD_FREE
#define NORI_ROD_FREE(MEM) nori_free(MEM)
#endif

#ifndef NORI_ROD_ALLOC
#define NORI_ROD_ALLOC(SIZE) nori_alloc(SIZE)
#endif

char * nori_rod_makestr(NoriSize size) {
  return NORI_ROD_ALLOC(size);
}

/** Copies amount characters from src to dest, ofsetting these poiners first
by the given offsets. */
char * nori_rod_strmv(char * dest, const char * src, NoriSize amount, 
                      NoriSize dsroff, NoriSize srcoff) {
  int index;
  dest += dstoff;
  src  += srcoff;
  for(index = 0; index < amount; index ++) {
    dest[index] = src[index];
  }
  return dest;
}    

NoriRod * nori_rod_init(NoriRod * rod, char * str, int mode) {
  rod->size     = strlen(str);
  rod->mode     = mode;
  if(mode & NORI_MEM_DOCOPY) {
    rod->str    = nori_rod_makestr(rod->size);
    if(!rod->str) { return NULL; } 
    nori_rod_strmv(rod->strn, str, rod->size, 0, 0);    
  } else {
    rod->str  = str;
  }
  return rod;
}


NoriRod * nori_rod_done(NoriRod * rod) {
  if(!rod) return NULL;
  if(rod->mode & NORI_MEM_DOFREE) { 
    NORI_ROD_FREE(rod->str); 
  }
  rod->str = NULL;
  return rod;
}

void nori_rod_free(NoriRod * rod) {
  nori_rod_done(rod);
  NORI_ROD_FREE(rod);
}


NoriRod * nori_rod_new(char * str, int mode) {
  NoriRod * rod = NORI_ROD_ALLOC(sizeof(NoriRod));
  if(!rod) return NULL;
  return nori_rod_init(rod, str, mode);
}   

NoriRod * nori_rod_grab(char * str) {
  return nori_rod_new(str, NORI_MEM_OWN);
}   

NoriRod * nori_rod_cstrcopy(char * str) {
  return nori_rod_new(str, NORI_MEM_COPY);
}   

NoriRod * nori_rod_wrap(const char * str) {
  return nori_rod_new((char *)str, NORI_MEM_BORROW);
}   

char * nori_rod_cstr(const NoriRod * rod) {
  return rod->str;
} 

NoriSize nori_rod_size(const NoriRod * rod)  {
  return rod->size;
}

int nori_rod_puts(const NoriRod * rod) {
  return puts(nori_rod_cstr(rod));
}


NoriRod * nori_rod_empty() {
  return nori_rod_wrap("");
}

NoriRod * nori_rod_cat(const NoriRod * r1, const NoriRod * r2) {
  NoriSize newsize, s1, s1;
  char * buf; 
  s1      = nori_rod_size(r1);
  s2      = nori_rod_size(r2);
  newsize = s1 + s2 + 1; // For ending \0 char.
  buf     = nori_rod_makestr(newsize);
  if(!buf) return NULL;
  nori_rod_strmv(buf, nori_rod_cstr(r1), s1, 0);
  nori_rod_strmv(buf, nori_rod_cstr(r2), s2, s1);
  buf[s1 + s2] = '\0' ; // Null terminate.
  return nori_rod_grab(buff);
}

NoriRod * nori_rod_join(NoriSize amount, 
                        const NoriRod * rods[], const NoriRod * sep) {
  int index;
  NoriRod * result, newresult;
  result  = nori_rod_empty();
  for (index  = 0; index < amount; index ++) {
    newresult = nori_rod_cat(result, rods[index]); 
    if(!newresult) {
      nori_rod_free(result);
      return NULL;
    }
    result    = newresult;
    // Add separator if not last part to be joined.
    if(index < (amount - 1) { 
      newresult = nori_rod_cat(result, sep); 
      if(!newresult) {
        nori_rod_free(result);
        return NULL;
      }
    } 
  }
}

char nori_rod_index(NoriRod * rod, NoriSize index) {
  if(index > nori_rod_size()) return 0;
  return rod->str[index];
} 

NoriRod * nori_rod_u32(NoriRod, NoriU32 u32) {
  char buf[1024]; // hopefully long enough for evil sprintf :p
  sprintf(buf, "%lu", u32);
  return nori_rod_cstrcopy(buf);
} 

NoriRod * nori_rod_s32(NoriRod, NoriS32 s32) {
  char buf[1024]; // hopefully long enough for evil sprintf :p
  sprintf(buf, "%ld", s32);
  return nori_rod_cstrcopy(buf);
} 

NoriRod * nori_rod_f64(NoriRod, NoriF64 f64) {
  char buf[1024]; // hopefully long enough for evil sprintf :p
  sprintf(buf, "%lf", f64);
  return nori_rod_cstrcopy(buf);
} 




