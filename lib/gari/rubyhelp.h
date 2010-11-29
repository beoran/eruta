/**
* rubyhelp.h 
* Helper macros for exporting C functions to MRI Ruby.
*/
#ifndef RUBYHELP_H
#define RUBYHELP_H


#include "ruby.h"
#include "ruby/intern.h"
#include "ruby/encoding.h"

#ifdef vsnprintf
  #undef vsnprintf
#endif

#ifdef vprintf
  #undef vprintf
#endif  

/* Macros */

#define RBH_GETSTRUCT_DEFINE(STRUCTNAME, FUNCNAME, KLASS, KLASSNAME) \
STRUCTNAME * FUNCNAME(VALUE obj)        \
{                                       \
  STRUCTNAME * st;                      \
  Data_Get_Struct(obj, STRUCTNAME, st); \
  return st;                            \
} 

/** Removed typecheck for speed and fun :) 
  if(!rb_obj_is_kind_of(obj, KLASS)){   \
    rb_raise(rb_eTypeError, "wrong argument type %s (expected " KLASSNAME ")", \
             rb_obj_classname(obj));    \
  }                                     \
*/

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

// transforms a boolean VALUE to an int
#define RBH_BOOL_INT(VAL) (((VAL) == Qnil) || ((VAL) == Qfalse) ? FALSE : TRUE)

// transforms a C value (pointer or int) to Qtrue of Qfalse
#define RBH_TOBOOL(VAL) ((VAL) ? Qtrue : Qfalse)


// Helper to define numerical constants 
#define RBH_NUM_CONST(MOD, NAME, VALUE) \
        rb_define_const(MOD, NAME, UINT2NUM(VALUE));
        
// Helps defining variable argument lists
#define RBH_VA2(A, B)             (A), (B)
#define RBH_VA3(A, B, C)          RBH_VA2(A, B), (C)
#define RBH_VA4(A, B, C, D)       RBH_VA3(A, B, C), (D)
#define RBH_VA5(A, B, C, D, E)    RBH_VA4(A, B, C, D), (E)
#define RBH_VA6(A, B, C, D, E, F) RBH_VA5(A, B, C, D, E), (F)
#define RBH_VA7(A, B, C, D, E, F, G) \
        RBH_VA6(A, B, C, D, E, F), (G)
#define RBH_VA8(A, B, C, D, E, F, G, H) \
        RBH_VA7(A, B, C, D, E, F, G), (H)

 
// Helper to raise a rb_eRuntimeError. Use together with the RBH_VA2 , RBH_VA3,
// RBH_VA4, etc macros
#define RBH_RAISE(FMT, VA_ARGS) \
        rb_raise(rb_eRuntimeError, FMT, VA_ARGS)

// Helper for RBH_TOSTR. Needed because of how the preprocessor works.
#define RBH_TOSTR_AID(VAL)             #VAL
// Stringify macro, always stringifies.
#define RBH_TOSTR(VAL)                 RBH_TOSTR_AID(VAL)

// Token pasting macro.
#define RBH_PASTE(FIRST, SECOND)       FIRST##SECOND
// String concatenation macro. Relies on the space being the C string
// concatenation operator.
#define RBH_JOINSTR(FIRST, SECOND)    (FIRST SECOND)
// String concatenation macro. Relies on the space being the C string
// concatenation operator.
#define RBH_JOINSTR3(A1, A2, A3)      (A1 A2 A3)

// Helper that defines the name of the variable that will store the Ruby module. 
#define RBH_MODULE_VAR(MOD) (m##MOD)
// Helper that defines the name of the variable that will store the Ruby class.
#define RBH_CLASS_VAR(KLASS) (c##KLASS)
// Helper that stringifies the name of the Ruby module.
#define RBH_MODULE_NAME(MOD) RBH_TOSTR(MOD)
// Helper that stringifies the name of the Ruby class.
#define RBH_CLASS_NAME(KLASS) RBH_TOSTR(KLASS)



#define RBH_MODULE(MOD) \
        RBH_MODULE_VAR(MOD) = rb_define_module(RBH_MODULE_NAME(MOD))

// Shorthand for rb_define_class_under
#define RBH_CLASS_UNDER(MOD, KLASS, SUPER) \
        RBH_CLASS_VAR(KLASS) =             \
        rb_define_class_under(RBH_MODULE_VAR(MOD), RBH_CLASS_NAME(KLASS), SUPER)
        
// Defines a Ruby class with rb_cObject as the superclass
#define RBH_MODULE_CLASS(MOD, KLASS) \
        RBH_CLASS_UNDER(MOD, KLASS, rb_cObject)
        
// Defines a Ruby class under a Ruby module, with a Ruby superclass.
#define RBH_MODULE_CLASS_SUPER(MOD, KLASS, SUPER) \
        RBH_CLASS_UNDER(MOD, KLASS, RBH_CLASS_VAR(SUPER))

// Helper that determines the name for the unwrap function.
#define RBH_UNWRAP_NAME(KLASS) RBH_PASTE(rbh_unwrap_, KLASS)

// Defines the unwrap function for a given Ruby class and C struct.
// Call it wth RBH_UNWRAP(KLASS, VALUE).
#define RBH_UNWRAP_DEFINE(KLASS, STRUCT) \
        RBH_GETSTRUCT_DEFINE(STRUCT, RBH_UNWRAP_NAME(KLASS), RBH_CLASS_VAR(KLASS), RBH_CLASS_NAME(KLASS))

// Helper to define Ruby modules.
#define RBH_MODULE_DEFINE(MOD) static VALUE RBH_MODULE_VAR(MOD) = Qnil

// Helper to define Ruby classes. Automagically defines a functon to be used 
// with RBH_UNWRAP. 
#define RBH_CLASS_DEFINE(KLASS, STRUCT) \
        static VALUE RBH_CLASS_VAR(KLASS) = Qnil ; \
        RBH_UNWRAP_DEFINE(KLASS, STRUCT)
          
#define RBH_UNWRAP_CLASS(KLASS, VALUE) \
        RBH_UNWRAP_NAME(KLASS)(VALUE)

// Wraps a pointer to a struct into a ruby class. Useful if a mark function 
// is needed.
#define RBH_WRAP_MARK(KLASS, PTR, MARK, FREE) \
        Data_Wrap_Struct( RBH_CLASS_VAR(KLASS), MARK, FREE, PTR)

// Wraps a pointer to a struct into a ruby class. Useful if no mark is needed,
// you only need to suply the free pointer.
// Also checks PTR, and returns Qnil if PTR is NULL.
#define RBH_WRAP(KLASS, PTR, FREE) \
        ( (PTR) ? Data_Wrap_Struct(RBH_CLASS_VAR(KLASS), 0, FREE, PTR) : Qnil) 
 

// Unwraps a VALUE of ruby class KLASS to a pointer of the C struct.
// It calls the function that RBH_UNWRAP_DEFINE generates
#define RBH_UNWRAP(KLASS, VALUE) \
        RBH_UNWRAP_NAME(KLASS)(VALUE)

// Converts a ruby VALUE to a C string pointer.
#define RBH_STRING(VALUE) StringValuePtr(VALUE)

// Converts a ruby numerical value to an int
#define RBH_INT(VALUE) NUM2INT(VALUE)

// Converts a ruby numerical value to an unsigned int
#define RBH_UINT(VALUE) NUM2UINT(VALUE)

// Converts a ruby numerical value to a uint8_t
#define RBH_UINT8(VALUE) ((uint8_t)(NUM2INT(VALUE)))

// Converts a ruby numerical value to a C string
#define RBH_CSTR(VALUE) ((RSTRING_PTR(VALUE)))


// Converts an integer to a Ruby number value 
#define RBH_INT_NUM(INT) INT2NUM(INT)
// Converts an unsigned integer to a Ruby number value 
#define RBH_UINT_NUM(INT) UINT2NUM(INT)

// Converts an uint8_t to a ruby number value
#define RBH_UINT8_NUM(UINT8) RBH_INT_NUM((int)(UINT8))
// Converts a double to a ruby VALUE
#define RBH_DOUBLE_NUM(DBL) DBL2NUM(DBL)

// Helper to define a singleton method (class method) for a class
#define RBH_SINGLETON_METHOD(KLASS, NAME, FUNC, ARITY) \
        rb_define_singleton_method(RBH_CLASS_VAR(KLASS), #NAME, FUNC, ARITY)
  
// Macro to define a method for a class 
#define RBH_METHOD(KLASS, NAME, FUNC, ARITY) \
        rb_define_method(RBH_CLASS_VAR(KLASS), #NAME, FUNC, ARITY)
  
// Macro to define numerical constants in a class 
#define RBH_CLASS_NUM_CONST(KLASS, NAME, VALUE) \
        rb_define_const(RBH_CLASS_VAR(KLASS), #NAME, UINT2NUM(VALUE));
  
// Macro to define numerical constants in a module 
#define RBH_MODULE_NUM_CONST(KLASS, NAME, VALUE) \
        rb_define_const(RBH_MODULE_VAR(KLASS), #NAME, UINT2NUM(VALUE));

/** Helper that determines the getter's name. Normally this is an rb 
* prefix to NAME.
*/
#define RBH_GETTER_NAME(NAME) rb##NAME

/** Macro that defines simple getter methods.
* Suppose you have some C struct Foo, that has a getter function 
* foo_get_bla(Foo * f). With this macro you can define a 
* function rbfoo_get_bla(VALUE self) that will call foo_get_bla, 
* and then wrap it into a VALUE using the WRAP function or macro you specify.  
*/
#define RBH_GETTER_DEFINE(NAME, KLASS, STRUCT, WRAP)  \
  VALUE RBH_GETTER_NAME(NAME) (VALUE self) {          \
  STRUCT * cself = RBH_UNWRAP(KLASS, self);           \
  return WRAP(NAME(cself));                           \
  }

// Macro to install a getter method that was defined with RBH_GETSTRUCT_DEFINE 
#define RBH_GETTER(KLASS, NAME, FUNC) \
        rb_define_method(RBH_CLASS_VAR(KLASS), #NAME, RBH_GETTER_NAME(FUNC), 0)


#endif
