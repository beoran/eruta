/*
* Amaru. Ruby graphics acceleration library for use wity Ruby/SDL 2.1.0
*/

#include <SDL.h>
#include <ruby.h>
#include <stdio.h>
#include <stdlib.h>

/* Macros */
#define GLOBAL_DEFINE_GET_STRUCT(struct_name, fun, klass, klassstr) \
struct_name* fun(VALUE obj) \
{ \
  struct_name* st; \
  \
  if(!rb_obj_is_kind_of(obj, klass)){ \
    rb_raise(rb_eTypeError, "wrong argument type %s (expected " klassstr ")", \
             rb_obj_classname(obj)); \
  } \
  Data_Get_Struct(obj, struct_name, st); \
  return st; \
} 

#define DEFINE_GET_STRUCT(struct_name, fun, klass, klassstr) \
static GLOBAL_DEFINE_GET_STRUCT(struct_name, fun, klass, klassstr)


#define SetRect(rect,X,Y,W,H) \
do{ \
  rect.x=NUM2INT(X); \
  rect.y=NUM2INT(Y); \
  rect.w=NUM2INT(W); \
  rect.h=NUM2INT(H); \
}while(0) \















