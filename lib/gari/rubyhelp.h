/**
* rubyhelp.h 
* Helper macros for exporting C functions to MRI Ruby.
*/
#ifndef RUBYHELP_H
#define RUBYHELP_H


#include "ruby.h"
#include "ruby/intern.h"
#include "ruby/encoding.h"

/* Macros */

#define RBH_GETSTRUCT_DEFINE(STRUCTNAME, FUNCNAME, KLASS, KLASSNAME) \
STRUCTNAME * FUNCNAME(VALUE obj)        \
{                                       \
  STRUCTNAME * st;                      \
                                        \
  if(!rb_obj_is_kind_of(obj, KLASS)){   \
    rb_raise(rb_eTypeError, "wrong argument type %s (expected " KLASSNAME ")", \
             rb_obj_classname(obj));    \
  }                                     \
  Data_Get_Struct(obj, STRUCTNAME, st); \
  return st;                            \
} 


#define RBH_STATIC_GETSTRUCT_DEFINE(struct_name, fun, klass, klassstr) \
static RBH_GETSTRUCT_DEFINE(struct_name, fun, klass, klassstr)

#define RBH_STRING_ENC(VAL, ENC)                   \
  do {                                             \
    SafeStringValue(VAL);                          \
    (VAL) = rb_str_export_to_enc((VAL),(ENC));     \
  } while (0)

#define RBH_INT2BOOL(INTVAL) ((INTVAL) ? Qtrue : Qfalse)


#define RBH_ENCSTR_NEW(str, enc) rb_enc_str_new((str), strlen((str)), (enc))



#endif