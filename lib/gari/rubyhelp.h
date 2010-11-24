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

#define RBH_UTF_ENC rbh_utf8_enc
#define RBH_UTF_ENC_DEFINE static rb_encoding * RBH_UTF_ENC = NULL;
#define RBH_ENCSTR_NEW(str, enc) rb_enc_str_new((str), strlen((str)), (enc))
#define RBH_ENCSTR_NEW2(str, enc) rb_enc_str_new((str), strlen((str)), (enc))

#define RBH_STR_ASCII(STR, LEN) rb_usascii_str_new(STR, LEN)
#define RBH_STR_ASCII2(STR) RBH_STR_ASCII(STR, strlen(STR))
#define RBH_STR_UTF8(STR, LEN) rb_enc_str_new(STR, LEN, rb_utf8_encoding())
#define RBH_STR_UTF82(STR) RBH_STR_UTF8(STR, strlen(STR))

#define RBH_WRAPOLD(KLASS, PTR, MARK, FREE)              \
        Data_Wrap_Struct(KLASS, MARK, FREE, PTR)
        
#define RBH_UNWRAPOLD(KLASS, PTR, VALUE) Data_Get_Struct(VALUE, KLASS, PTR)

#define RBH_MAKE(KLASS, STRUCT, PTR) \
        Data_Make_Struct(KLASS, STRUCT, 0, -1, PTR)

// transforms a boolean VALUE to an int)
#define RBH_BOOL2INT(VAL) (((VAL) == Qnil) || ((VAL) == Qfalse) ? FALSE : TRUE)

// transforms a C value (pointer or int) to Qtrue of Qfalse
#define RBH_TOBOOL(VAL) ((VAL) ? Qtrue : Qfalse)


// Helper to define numerical constants 
#define RBH_NUM_CONST(MOD, NAME, VALUE) \
        rb_define_const(MOD, NAME, UINT2NUM(VALUE));
 
// Helper to raise a rb_eRuntimeError. Requires a C compiler 
// that understands __VA_ARGS__ 
#define RBH_RAISE(FMT, ...) \
        rb_raise(rb_eRuntimeError, FMT, __VA_ARGS__)

#define RBH_TOSTR_AID(VAL)             #VAL
#define RBH_TOSTR(VAL)                 RBH_TOSTR_AID(VAL)

#define RBH_PASTE(FIRST, SECOND)       FIRST##SECOND
#define RBH_JOINSTR(FIRST, SECOND)    (FIRST SECOND)
#define RBH_JOINSTR3(A1, A2, A3)      (A1 A2 A3)


#define RBH_MODULE_VAR(MOD) (m##MOD)
#define RBH_CLASS_VAR(KLASS) (c##KLASS)
#define RBH_MODULE_NAME(MOD) RBH_TOSTR(MOD)
#define RBH_CLASS_NAME(KLASS) RBH_TOSTR(KLASS)



#define RBH_MODULE(MOD) \
        RBH_MODULE_VAR(MOD) = rb_define_module(RBH_MODULE_NAME(MOD))
        
#define RBH_CLASS_UNDER(MOD, KLASS, SUPER) \
        RBH_CLASS_VAR(KLASS) =             \
        rb_define_class_under(RBH_MODULE_VAR(MOD), RBH_CLASS_NAME(KLASS), SUPER)
        
#define RBH_MODULE_CLASS(MOD, KLASS) \
        RBH_CLASS_UNDER(MOD, KLASS, rb_cObject)
        
#define RBH_MODULE_CLASS_SUPER(MOD, KLASS, SUPER) \
        RBH_CLASS_UNDER(MOD, KLASS, RBH_CLASS_NAME(SUPER))

#define RBH_UNWRAP_NAME(KLASS) RBH_PASTE(rbh_unwrap_, KLASS)

#define RBH_UNWRAP_DEFINE(KLASS, STRUCT) \
        RBH_GETSTRUCT_DEFINE(STRUCT, RBH_UNWRAP_NAME(KLASS), RBH_CLASS_VAR(KLASS), RBH_CLASS_NAME(KLASS))

#define RBH_MODULE_DEFINE(MOD) static VALUE RBH_MODULE_VAR(MOD) = NULL 
#define RBH_CLASS_DEFINE(KLASS, STRUCT) \
        static VALUE RBH_CLASS_VAR(KLASS) = NULL ; \
        RBH_UNWRAP_DEFINE(KLASS, STRUCT)
          
          
#define RBH_UNWRAP_CLASS(KLASS, VALUE) \
        RBH_UNWRAP_NAME(KLASS)(VALUE)

#define RBH_WRAP_NAME(KLASS) RBH_PASTE(rbh_wrap_, KLASS)

#define RBH_WRAP_MARK(KLASS, PTR, MARK, FREE) \
        Data_Wrap_Struct( RBH_CLASS_VAR(KLASS), PTR, MARK, FREE)

#define RBH_WRAP(KLASS, PTR, FREE) \
        Data_Wrap_Struct( RBH_CLASS_VAR(KLASS), PTR, NULL, FREE)

#define RBH_UNWRAP(KLASS, VALUE) \
        RBH_UNWRAP_NAME(KLASS)(VALUE)

#define RBH_STRING(VALUE) StringValuePtr(VALUE)
#define RBH_INT(VALUE) NUM2INT(VALUE)

#define RBH_SINGLETON_METHOD(KLASS, NAME, FUNC, ARITY) \
        rb_define_singleton_method(RBH_CLASS_VAR(KLASS), #NAME, FUNC, ARITY)

#define RBH_METHOD(KLASS, NAME, FUNC, ARITY) \
        rb_define_method(RBH_CLASS_VAR(KLASS), #NAME, FUNC, ARITY)
  
// Helper to define numerical constants in a class 
#define RBH_CLASS_NUM_CONST(KLASS, NAME, VALUE) \
        rb_define_const(RBH_CLASS_VAR(KLASS), #NAME, UINT2NUM(VALUE));
  

#endif
