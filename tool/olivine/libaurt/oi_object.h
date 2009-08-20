#ifndef OI_OBJ_H
#define OI_OBJ_H

#include "oi_memory.h"
#include "oi_method.h"
#include "oi_member.h"

/*
struct oi_object_     ;
struct oi_memory_     ;
struct oi_methods_    ; 
struct oi_members_    ;
*/


struct oi_object_ {
  struct oi_memory_  memory;
  struct oi_methods_ methods;
  struct oi_members_ members;
};

typedef struct oi_object_ oi_object;





#endif
