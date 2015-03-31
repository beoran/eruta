#ifndef tr_thing_H_INCLUDED
#define tr_thing_H_INCLUDED

/* Macros to generate Thing-reated functionality. */

#define TR_THING_GET(THING, STATE, THINGID)                                    \
  THING = state_thing(STATE, THINGID);                                         \
  if (!THING) {                                                                \
    return mrb_nil_value();                                                    \
  }                                                                            

#define TR_THING_FUNC_INIT(THING, STATE)                                       \
  State * STATE    = state_get();                                              \
  Thing * THING    = NULL;                                                     \
  (void) self;                                                                 

#define TR_THING_IGETTER(NAME, TOCALL)                                         \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  TR_THING_FUNC_INIT(thing, state)                                             \
  int result;                                                                  \
  mrb_int thingid;                                                             \
  mrb_get_args(mrb, "i", &thingid);                                            \
  TR_THING_GET(thing, state, thingid);                                         \
  return mrb_fixnum_value(TOCALL(thing));                                      \
} 


#define TR_THING_II_INT(NAME, TOCALL)                                          \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  TR_THING_FUNC_INIT(thing, state)                                             \
  mrb_int result;                                                              \
  mrb_int thingid, i1, i2;                                                     \
  mrb_get_args(mrb, "iii", &thingid, &i1, &i2);                                \
  TR_THING_GET(thing, state, thingid);                                         \
  result = TOCALL(thing, i1, i2);                                              \
  return mrb_fixnum_value(result);                                             \
}


#define TR_THING_I_INT(NAME, TOCALL)                                           \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  TR_THING_FUNC_INIT(thing, state)                                             \
  mrb_int result;                                                              \
  mrb_int thingid, i1;                                                         \
  mrb_get_args(mrb, "ii", &thingid, &i1);                                      \
  TR_THING_GET(thing, state, thingid);                                         \
  result = TOCALL(thing, i1);                                                  \
  return mrb_fixnum_value(result);                                             \
}

#define TR_THING_I_BOOL(NAME, TOCALL)                                          \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  TR_THING_FUNC_INIT(thing, state)                                             \
  mrb_int result;                                                              \
  mrb_int thingid, i1;                                                         \
  mrb_get_args(mrb, "ii", &thingid, &i1);                                      \
  TR_THING_GET(thing, state, thingid);                                         \
  result = TOCALL(thing, i1);                                                  \
  return rh_bool_value(result);                                                \
}


#define TR_THING_III_INT(NAME, TOCALL)                                         \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  TR_THING_FUNC_INIT(thing, state)                                             \
  mrb_int result;                                                              \
  mrb_int thingid,i1, i2, i3;                                                  \
  mrb_get_args(mrb, "iiii", &thingid, &i1, &i2, &i3);                          \
  TR_THING_GET(thing, state, thingid);                                         \
  result = TOCALL(thing, i1, i2, i3);                                          \
  return mrb_fixnum_value(result);                                             \
}


#define TR_AREA_FUNC_INIT(AREA, STATE)                                         \
  State * STATE    = state_get();                                              \
  Area  * AREA     = state_area(STATE);                                        \
  (void) self;                                                                 


#define TR_AREA_II_BOOL(NAME, TOCALL)                                          \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  TR_AREA_FUNC_INIT(area, state)                                               \
  mrb_int result;                                                              \
  mrb_int i1, i2;                                                              \
  mrb_get_args(mrb, "ii", &i1, &i2);                                           \
  result = TOCALL(area, i1, i2);                                               \
  return rh_bool_value(result);                                                \
}

/* Macros to generate wrappers for area related functionality. */

#define TR_AREA_I_INT(NAME, TOCALL)                                            \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  TR_AREA_FUNC_INIT(area, state)                                               \
  mrb_int result;                                                              \
  mrb_int i1;                                                                  \
  mrb_get_args(mrb, "i", &i1);                                                 \
  result = TOCALL(area, i1);                                                   \
  return mrb_fixnum_value(result);                                             \
}


#define TR_AREA_II_INT(NAME, TOCALL)                                           \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  TR_AREA_FUNC_INIT(area, state)                                               \
  mrb_int result;                                                              \
  mrb_int i1, i2;                                                              \
  mrb_get_args(mrb, "ii", &i1, &i2);                                           \
  result = TOCALL(area, i1, i2);                                               \
  return mrb_fixnum_value(result);                                             \
}


#define TR_AREA_III_INT(NAME, TOCALL)                                          \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  TR_AREA_FUNC_INIT(area, state)                                               \
  mrb_int result;                                                              \
  mrb_int i1, i2, i3;                                                          \
  mrb_get_args(mrb, "iii", &i1, &i2, &i3);                                     \
  result = TOCALL(area, i1, i2, i3);                                           \
  return mrb_fixnum_value(result);                                             \
}


#define TR_AREA_IIII_INT(NAME, TOCALL)                                         \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {                       \
  TR_AREA_FUNC_INIT(area, state)                                               \
  mrb_int result;                                                              \
  mrb_int i1, i2, i3, i4;                                                      \
  mrb_get_args(mrb, "iiii", &i1, &i2, &i3, &i4);                               \
  result = TOCALL(area, i1, i2, i3, i4);                                       \
  return mrb_fixnum_value(result);                                             \
}



int tr_thing_init(mrb_state * mrb, struct RClass * eru);


#endif




