/*
 * This file contains macros to help generate the bindings
 * of functionality to MRuby.
 */

#ifndef TR_MACRO_H_INCLUDED
#define TR_MACRO_H_INCLUDED

#define RH_WRAP(RUBY, TYPE, OBJECT)                         \
mrb_obj_value(Data_Wrap_Struct(RUBY,                        \
              RUBY->object_class, &toruby_##TYPE, OBJECT));

#define TR_NAME(NAME)               tr_##NAME
#define TR_PAIR_HELPER(NAME)        TR_NAME(NAME), NAME
#define TR_PAIR(NAME)               TR_PAIR_HELPER(NAME)
#define TR_MACRO_AID(MACRO, ARGS)   MACRO ARGS        
#define TR_PAIR_DO_AID(MACRO, NAME) TR_MACRO_AID(MACRO, (TR_PAIR(NAME)))
#define TR_PAIR_DO(MACRO, NAME)     TR_PAIR_DO_AID(MACRO, NAME)

#define TORUBY_0_ICALLER(NAME, TOCALL)                                         \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                     \
  (void) self; (void) mrb;                                                     \
  return mrb_fixnum_value(TOCALL());                                           \
}

#define TORUBY_0_FGETTER(NAME, TOCALL)                                         \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                     \
  (void) self;                                                                 \
  return mrb_float_value(mrb, TOCALL());                                       \
}

#define SCEGRA_ICALLER(NAME, TOCALL)                                           \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                     \
  int index;                                                                   \
  (void) self;                                                                 \
  mrb_get_args(mrb, "i", &index);                                              \
  return mrb_fixnum_value(TOCALL(index));                                      \
}

#define SCEGRA_BCALLER(NAME, TOCALL)                                           \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                     \
  int index;                                                                   \
  (void) self;                                                                 \
  mrb_get_args(mrb, "i", &index);                                              \
  return rh_bool_value(TOCALL(index));                                         \
}


#define SCEGRA_FCALLER(NAME, TOCALL)                                           \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                     \
  mrb_int index;                                                               \
  (void) self;                                                                 \
  mrb_get_args(mrb, "i", &index);                                              \
  return mrb_float_value(mrb, TOCALL(index));                                  \
  }

#define SCEGRA_ISETTER(NAME, TOCALL)                                           \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                     \
  int index, value;                                                            \
  (void) self;                                                                 \
  mrb_get_args(mrb, "ii", &index, &value);                                     \
  return mrb_fixnum_value(TOCALL(index, value));                               \
}

#define SCEGRA_BSETTER(NAME, TOCALL)                                           \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                     \
  int index; mrb_value value;                                                  \
  (void) self;                                                                 \
  mrb_get_args(mrb, "io", &index, &value);                                     \
  return mrb_fixnum_value(TOCALL(index, rh_tobool(value)));                    \
}
  
#define SCEGRA_PSETTER(NAME, TOCALL)                                           \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                     \
  int index, x, y;                                                             \
  (void) self;                                                                 \
  mrb_get_args(mrb, "iii", &index, &x, &y);                                    \
  return mrb_fixnum_value(TOCALL(index, x, y));                                \
}

#define SCEGRA_CSETTER(NAME, TOCALL)                                           \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                     \
  int index, r, g, b, a;                                                       \
  (void) self;                                                                 \
  mrb_get_args(mrb, "iiiii", &index, &r, &g, &b, &a);                          \
  return mrb_fixnum_value(TOCALL(index, r, g, b, a));                          \
}

#define SCEGRA_FSETTER(NAME, TOCALL)                                           \
  static mrb_value NAME(mrb_state * mrb, mrb_value self) {                     \
  int index; mrb_float value;                                                  \
  (void) self;                                                                 \
  mrb_get_args(mrb, "if", &index, &value);                                     \
  return mrb_fixnum_value(TOCALL(index, value));                               \
}

#define TR_WRAP_NOARG_BOOL(NAME, TOCALL)                                       \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  (void) self; (void) mrb;                                                     \
  return rh_bool_value(TOCALL());                                              \
}

#define TR_WRAP_I_BOOL(NAME, TOCALL)                                           \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  mrb_int i1;                                                                  \
  (void) self;                                                                 \
  mrb_get_args(mrb, "i", &i1);                                                 \
  return rh_bool_value(TOCALL(i1));                                            \
}

#define TR_WRAP_B_BOOL(NAME, TOCALL)                                           \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  mrb_value b1;                                                                \
  (void) self;                                                                 \
  mrb_get_args(mrb, "o", &b1);                                                 \
  return rh_bool_value(TOCALL(rh_tobool(b1)));                                 \
}

#define TR_WRAP_IZ_INT(NAME, TOCALL)                                           \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  mrb_int i1 = -1;                                                             \
  char * str = NULL;                                                           \
  (void) self;                                                                 \
  mrb_get_args(mrb, "iz", &i1, &str);                                          \
  return mrb_fixnum_value(TOCALL(i1, str));                                    \
}


#define TR_WRAP_I_INT(NAME, TOCALL)                                            \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  mrb_int i1;                                                                  \
  mrb_get_args(mrb, "i", &i1);                                                 \
  (void) self;                                                                 \
  return mrb_fixnum_value(TOCALL(i1));                                         \
}

#define TR_WRAP_II_INT(NAME, TOCALL)                                           \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  mrb_int i1, i2;                                                              \
  mrb_get_args(mrb, "ii", &i1, &i2);                                           \
  (void) self;                                                                 \
  return mrb_fixnum_value(TOCALL(i1, i2));                                     \
}

#define TR_WRAP_III_INT(NAME, TOCALL)                                          \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  mrb_int i1, i2, i3;                                                          \
  mrb_get_args(mrb, "iii", &i1, &i2, &i3)                                      \
  (void) self;                                                                 \
  return mrb_fixnum_value(TOCALL(i1, i2, i3));                                 \
}


#define TR_WRAP_IIII_INT(NAME, TOCALL)                                         \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  mrb_int i1, i2, i3, i4;                                                      \
  mrb_get_args(mrb, "iiii", &i1, &i2, &i3, &i4);                               \
  (void) self;                                                                 \
  return mrb_fixnum_value(TOCALL(i1, i2, i3, i4));                             \
}

#define TR_WRAP_IIIIB_INT(NAME, TOCALL)                                        \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  mrb_int i1, i2, i3, i4;                                                      \
  mrb_value b5;                                                                \
  (void) self;                                                                 \
  mrb_get_args(mrb, "iiiib", &i1, &i2, &i3, &i4, &b5);                         \
  return mrb_fixnum_value(TOCALL(i1, i2, i3, i4, rh_tobool(b5)));              \
}


#define TR_WRAP_NOARG_INT(NAME, TOCALL)                                        \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  (void) self; (void) mrb;                                                     \
  return mrb_fixnum_value(TOCALL());                                           \
}


#define TR_SPRITE_GET(SPRITE, STATE, SPRITEID)                                 \
  SPRITE = state_sprite(STATE, SPRITEID);                                      \
  if (!SPRITE) {                                                               \
    return mrb_nil_value();                                                    \
  }                                                                            

#define TR_SPRITE_FUNC_INIT(SPRITE, STATE, SPRITEID)                           \
  mrb_int SPRITEID = -1;                                                       \
  State * STATE    = state_get();                                              \
  Sprite * sprite  = NULL;                                                     \
  (void) self;                                                                 


#define TR_SPRITE_II_INT(NAME, TOCALL)                                         \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  TR_SPRITE_FUNC_INIT(sprite, state, index)                                    \
  mrb_int result;                                                              \
  mrb_int i1, i2;                                                              \
  mrb_get_args(mrb, "iii", &index, &i1, &i2);                                  \
  TR_SPRITE_GET(sprite, state, index);                                         \
  result = TOCALL(sprite, i1, i2);                                             \
  return mrb_fixnum_value(result);                                             \
}




#define TR_METHOD(MRB, CLASS, NAME, IMPL, FLAGS)                               \
        mrb_define_method((MRB), (CLASS), (NAME), (IMPL), (FLAGS))

#define TR_METHOD_ARGC(MRB, CLASS, NAME, IMPL, ARGC)\
        mrb_define_method((MRB), (CLASS), (NAME), (IMPL), ARGS_REQ(ARGC))

#define TR_METHOD_NOARG(MRB, CLASS, NAME, IMPL)\
        mrb_define_method((MRB), (CLASS), (NAME), (IMPL), ARGS_NONE())

#define TR_METHOD_OPTARG(MRB, CLASS, NAME, IMPL, ARGC, OPTC) \
mrb_define_method((MRB), (CLASS), (NAME), (IMPL), ARGS_REQ(ARGC) | ARGS_OPT(OPTC))

#define TR_CLASS_METHOD(MRB, CLASS, NAME, IMPL, FLAGS)\
        mrb_define_class_method((MRB), (CLASS), (NAME), (IMPL), (FLAGS))

#define TR_CLASS_METHOD_ARGC(MRB, CLASS, NAME, IMPL, ARGC)\
        mrb_define_class_method((MRB), (CLASS), (NAME), (IMPL), ARGS_REQ(ARGC))

#define TR_CLASS_METHOD_NOARG(MRB, CLASS, NAME, IMPL)\
        mrb_define_class_method((MRB), (CLASS), (NAME), (IMPL), ARGS_NONE())

#define TR_CLASS_METHOD_OPTARG(MRB, CLASS, NAME, IMPL, ARGC, OPTC) \
mrb_define_class_method((MRB), (CLASS), (NAME), (IMPL), ARGS_REQ(ARGC) | ARGS_OPT(OPTC))



#define TR_CONST_INT(MRB, CLASS, NAME, VALUE) \
      mrb_define_const((MRB), (CLASS), (NAME), mrb_fixnum_value(VALUE))

#define TR_CONST_INT_EASY(MRB, CLASS, PREFIX, NAME) \
        TR_CONST_INT(MRB, CLASS, #NAME, PREFIX##NAME)


#endif // TR_H_INCLUDED
