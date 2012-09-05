#ifndef obj_H_INCLUDED
#define obj_H_INCLUDED

#include "mem.h"


/** OBJ imlementa a small OOP-like system with 
reference counted memory that can still be used,
if neccesary like plain pointers. */



/** OOP style class info. The most important 
values are done aqnd free. Done should release all
references the object holds, and may be called several times 
recursively. Free should deallocate the memory used by 
the object itself. The fiel up may be NULL, but if it is not,
it should contain a pointer to a parent class. done and free
will be looked up recursively in the parent class if not defined 
in the child class */
struct ObjClass_;
typedef struct ObjClass_ ObjClass;


/** ObjMethod is a generic object method pointer. It's assumed that as 
required by the ANSI C standard that all function pointers have the 
same size, so they can be cast to the required real method signature */

typedef ObjClass * (ObjMethod)(ObjClass * self, ...); 

/** This macro can be used to implement your own object classes.
Alternatively, include a member ObjClass as the first member of your 
own claqss. */
#define IMPLEMENT_OBJCLASS            \
        struct ObjClass_   * up;      \
        ObjMethod          * done;    \
        ObjMethod          * free;


/** The object class structure. */
struct ObjClass_ {
  struct ObjClass_   * up;
  ObjMethod          * done;
  ObjMethod          * free;
};

/** OBJCLASS_GETFUNCTION_AT looks up a function pointer in the given
klasspointer. */
#define OBJCLASS_GETFUNCTION_AT(KLASSPTR, FUNCTYPE, OFFSET) \
        ((FUNCTYPE)(((char *)(KLASSPTR))+(OFFSET)))


/** OBJCLASS_GETFUNCTION uses offsetof to look up the named function pointer 
* of the given klass. KLASSTYPE should be the name of the struct that
* KLASSPTR points to.
*/
#define OBJCLASS_GETFUNCTION(KLASSPTR, KLASSTYPE, FUNCTYPE, NAME)      \
        OBJCLASS_GETFUNCTION_AT((KLASSPTR), (FUNCTYPE),     \
                                offsetof(KLASSTYPE, NAME))


/** An object pool allows several objects to be allocated in turn, and then 
all be unreferenced at once. */
struct ObjPool_;
typedef struct ObjPool_ ObjPool; 


struct ObjPoolNode_;
typedef struct ObjPoolNode_ ObjPoolNode;

/** Object pool struct. Use only as opaque object. */
struct ObjPool_ {
  ObjPoolNode * last; 
  // last object registered to the pool, forms a linked list. 
};



#include "obj_proto.h"




#endif




