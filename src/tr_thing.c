
#include "eruta.h"
#include "toruby.h"
#include "rh.h"
#include "state.h"
#include "image.h"
#include "fifi.h"
#include "store.h"
#include "scegra.h"
#include "sound.h"
#include <mruby/hash.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/array.h>
#include "tr_macro.h"
#include "tr_thing.h"

static mrb_value tr_newthing(mrb_state * mrb, mrb_value self) {
  int thing  = -1;
  State * state    = state_get();
  int result;
  mrb_int kind, x, y, z, w, h;
  (void) self;
  mrb_get_args(mrb, "iiiiii", &kind, &x, &y, &z, &w, &h);  
  if (kind < 0) {
    return mrb_nil_value();
  } 
  thing = state_newthingindex(state, kind, x, y, z, w, h);
  return mrb_fixnum_value(thing);
}


static mrb_value tr_thing_sprite_(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result;
  mrb_int thing, sprite;
  (void) self;
  mrb_get_args(mrb, "ii", &thing, &sprite);  
  result = state_thing_sprite_(state, thing, sprite);
  return mrb_fixnum_value(result);
}

static mrb_value tr_thing_pose_(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result;
  mrb_int thing, pose;
  (void) self;
  mrb_get_args(mrb, "ii", &thing, &pose);  
  result = state_thing_pose_(state, thing, pose);
  return mrb_fixnum_value(result);
}

static mrb_value tr_thing_direction_(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result;
  mrb_int thing, direction;
  (void) self;
  mrb_get_args(mrb, "ii", &thing, &direction);  
  result = state_thing_direction_(state, thing, direction);
  return mrb_fixnum_value(result);
}

static mrb_value tr_thing_v_(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  Thing * thing    = NULL;
  int result;
  mrb_int thingid; mrb_float x, y;
  (void) self;
  mrb_get_args(mrb, "iff", &thingid, &x, &y); 
  thing = state_thing(state, thingid);
  if (!thing) {
    return mrb_nil_value();
  }
  thing_v_(thing, bevec(x , y)); 
  return mrb_fixnum_value(thingid);
}

struct tr_thing_find_helper {
  mrb_state * mrb;
  mrb_value * results;
};

static int tr_thing_find_callback(Thing * thing, void * extra) {
  struct tr_thing_find_helper * helper = extra;
  mrb_value id = mrb_fixnum_value(thing_id(thing));
  mrb_ary_push(helper->mrb,  (*helper->results), id);
  return 0;
}

static mrb_value tr_thing_find_in_rectangle
  (mrb_state * mrb, mrb_value self) {
  mrb_value results;
  int x, y, w, h;
  struct tr_thing_find_helper helper;
  State * state    = state_get();
  Area * area      = state_area(state);
  (void) self;
  
  mrb_get_args(mrb, "iiii", &x, &y, &w, &h);  
  results          = mrb_ary_new(mrb);
  helper.mrb       = mrb;
  helper.results   = &results;

  area_find_things(area, x, y, w, h, &helper, tr_thing_find_callback);
  return results;
}


/* Converts a bevec to an array of 2 floats */
mrb_value bevec2mrb(mrb_state * mrb, BeVec vec) { 
    mrb_value vals[2];
    mrb_value marr;
    vals[0] = mrb_fixnum_value(vec.x);
    vals[1] = mrb_fixnum_value(vec.y);
    marr = mrb_ary_new_from_values(mrb, 2, vals);
    return marr;
}

static mrb_value tr_thing_v(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  Thing * thing    = NULL;
  BeVec result;
  mrb_int thingid; mrb_float x, y;
  (void) self;
  mrb_get_args(mrb, "i", &thingid); 
  thing = state_thing(state, thingid);
  if (!thing) {
    return mrb_nil_value();
  }
  result = thing_v(thing); 
  return bevec2mrb(mrb, result);
}


static mrb_value tr_thing_tint(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result       = 0;
  mrb_int   rindex = -1;
  mrb_int   rlayer = -1;
  mrb_int   rr     = 255;
  mrb_int   rg     = 255;
  mrb_int   rb     = 255;
  mrb_int   ra     = 255;
  (void) self;
  mrb_get_args(mrb, "iiiiii", &rindex, &rlayer, &rr, &rg, &rb, &ra); 
  if ((rindex<0) || (rlayer<0)) {
    return mrb_nil_value();
  }
  result = 
  state_thing_tint_layer(state, rindex, rlayer, rr, rg, rb, ra);
  return mrb_fixnum_value(result);
}

TR_THING_II_INT(tr_thing_hide_layer, thing_hide_layer)
TR_THING_I_INT(tr_thing_is_layer_hidden, thing_is_layer_hidden)

TR_THING_II_INT(tr_thing_set_action_loop, thing_set_action_loop)
TR_THING_I_INT(tr_thing_get_action_loop, thing_get_action_loop)
TR_THING_I_BOOL(tr_thing_is_action_done, thing_is_action_done)
TR_THING_III_INT(tr_thing_set_pose_direction_loop, thing_set_pose_direction_loop)
TR_THING_II_INT(tr_thing_get_pose_direction_loop, thing_get_pose_direction_loop)


/* Define getters for various dimensions of thing's bounds box. */
TR_PAIR_DO(TR_THING_IGETTER, thing_w)
TR_PAIR_DO(TR_THING_IGETTER, thing_h)
TR_PAIR_DO(TR_THING_IGETTER, thing_x)
TR_PAIR_DO(TR_THING_IGETTER, thing_y)
TR_PAIR_DO(TR_THING_IGETTER, thing_cx)
TR_PAIR_DO(TR_THING_IGETTER, thing_cy)
TR_PAIR_DO(TR_THING_IGETTER, thing_z)
TR_PAIR_DO(TR_THING_IGETTER, thing_direction)
TR_PAIR_DO(TR_THING_IGETTER, thing_pose)

TR_WRAP_I_INT(tr_thing_get_unused_id, state_get_unused_thing_id);


TR_AREA_I_INT(tr_area_hull_flags, area_hull_flags);
TR_AREA_II_INT(tr_area_hull_flags_, area_hull_flags_);
TR_AREA_II_INT(tr_area_set_hull_flag, area_set_hull_flag);
TR_AREA_II_INT(tr_area_unset_hull_flag,  area_unset_hull_flag);

TR_AREA_I_INT(tr_area_thing_hull_flags, area_thing_hull_flags);
TR_AREA_II_INT(tr_area_thing_hull_flags_, area_thing_hull_flags_);
TR_AREA_II_INT(tr_area_set_thing_hull_flag, area_set_thing_hull_flag);
TR_AREA_II_INT(tr_area_unset_thing_hull_flag,  area_unset_thing_hull_flag);



/** Initialize mruby bindings to the physics engine and physical thing
 * functionality. Eru is the parent module, which is normally named "Eruta" on the
 * ruby side. */
int tr_thing_init(mrb_state * mrb, struct RClass * eru) {
  struct RClass *thi;
  struct RClass *krn;
  struct RClass *kin;
  struct RClass *fla;
  
  
  /* Thing class/module and class/module methods. */
  thi = mrb_define_class_under(mrb, eru, "Thing" , mrb->object_class);
  krn = mrb_module_get(mrb, "Kernel");
  kin = mrb_define_module_under(mrb, thi, "Kind");
  fla = mrb_define_module_under(mrb, thi, "Flag");


  TR_METHOD_ARGC(mrb, krn, "thing_new"    , tr_newthing, 7);


  TR_CLASS_METHOD_ARGC(mrb, thi, "thing_new", tr_newthing, 7);
  TR_CLASS_METHOD_ARGC(mrb, thi, "v"        , tr_thing_v , 1);
  TR_CLASS_METHOD_ARGC(mrb, thi, "v_"       , tr_thing_v_, 3);
  TR_CLASS_METHOD_ARGC(mrb, thi, "find_in_rectangle", tr_thing_find_in_rectangle, 4);


  TR_CLASS_METHOD_ARGC(mrb, thi, "sprite_", tr_thing_sprite_, 2);
  TR_CLASS_METHOD_ARGC(mrb, thi, "pose_"  , tr_thing_pose_, 2);
  TR_CLASS_METHOD_ARGC(mrb, thi, "direction_", tr_thing_direction_, 2);

  TR_CLASS_METHOD_NOARG(mrb, thi, "x" , tr_thing_x);
  TR_CLASS_METHOD_NOARG(mrb, thi, "y" , tr_thing_y);
  TR_CLASS_METHOD_NOARG(mrb, thi, "z" , tr_thing_z);
  TR_CLASS_METHOD_NOARG(mrb, thi, "cx", tr_thing_cx);
  TR_CLASS_METHOD_NOARG(mrb, thi, "cy", tr_thing_cy);
  TR_CLASS_METHOD_NOARG(mrb, thi, "h" , tr_thing_h);
  TR_CLASS_METHOD_NOARG(mrb, thi, "w" , tr_thing_w);
  TR_CLASS_METHOD_NOARG(mrb, thi, "direction", tr_thing_direction);
  TR_CLASS_METHOD_NOARG(mrb, thi, "pose"     , tr_thing_pose);
  TR_CLASS_METHOD_ARGC(mrb, thi, "get_unused_id"  , tr_thing_get_unused_id, 1);
  TR_CLASS_METHOD_ARGC(mrb, thi, "tint_rgba"      , tr_thing_tint, 6);
  TR_CLASS_METHOD_ARGC(mrb, thi, "hide_layer"     , tr_thing_hide_layer, 3);
  TR_CLASS_METHOD_ARGC(mrb, thi, "layer_hidden?"  , tr_thing_is_layer_hidden, 2);
  TR_CLASS_METHOD_ARGC(mrb, thi, "set_action_loop", tr_thing_set_action_loop, 3);
  TR_CLASS_METHOD_ARGC(mrb, thi, "get_action_loop", tr_thing_get_action_loop, 2);
  TR_CLASS_METHOD_ARGC(mrb, thi, "action_done?"   , tr_thing_is_action_done, 3);
  TR_CLASS_METHOD_ARGC(mrb, thi, "set_pose_direction_loop", tr_thing_set_pose_direction_loop, 4);
  TR_CLASS_METHOD_ARGC(mrb, thi, "get_pose_direction_loop", tr_thing_get_pose_direction_loop, 3);
  TR_CLASS_METHOD_ARGC(mrb, thi, "set_hull_flag"  , tr_area_set_thing_hull_flag, 2);
  TR_CLASS_METHOD_ARGC(mrb, thi, "unset_hull_flag", tr_area_unset_thing_hull_flag, 2);
  TR_CLASS_METHOD_ARGC(mrb, thi, "hull_flags"     , tr_area_thing_hull_flags, 2);
  TR_CLASS_METHOD_ARGC(mrb, thi, "hull_flags_"    , tr_area_thing_hull_flags_, 2);
  
  
  /* Collision types of a thing */
  TR_CONST_INT_EASY(mrb, fla, BUMP_FLAG_, NORMAL);
  TR_CONST_INT_EASY(mrb, fla, BUMP_FLAG_, SENSOR);
  TR_CONST_INT_EASY(mrb, fla, BUMP_FLAG_, DISABLED);
  TR_CONST_INT_EASY(mrb, fla, BUMP_FLAG_, USER1);
  TR_CONST_INT_EASY(mrb, fla, BUMP_FLAG_, USER2);
  TR_CONST_INT_EASY(mrb, fla, BUMP_FLAG_, USER3);
  TR_CONST_INT_EASY(mrb, fla, BUMP_FLAG_, USER4);

  TR_CONST_INT_EASY(mrb, kin, BUMP_KIND_, PLAYER);
  TR_CONST_INT_EASY(mrb, kin, BUMP_KIND_, NPC);
  TR_CONST_INT_EASY(mrb, kin, BUMP_KIND_, FOE);
  TR_CONST_INT_EASY(mrb, kin, BUMP_KIND_, ATTACK);
  TR_CONST_INT_EASY(mrb, kin, BUMP_KIND_, ART);
  TR_CONST_INT_EASY(mrb, kin, BUMP_KIND_, SEARCH);
 
  return 0;
}

