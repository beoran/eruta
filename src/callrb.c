
/* Callbacks from the C side into the mruby side.
 * Used to signal several events such as collisions or sprite
 * animation.
*/
#include "state.h"
#include "rh.h"
#include "spritestate.h"
#include "callrb.h"


/* Collision handler for things. Calls an mruby callback. */
int callrb_collide_things(Thing * t1, Thing * t2, int kind, void * data) { 
  mrb_value res;
  State * state;
  int ti1, ti2;
  Ruby * ruby;
  ti1       = thing_id(t1);
  ti2       = thing_id(t2);
  state     = state_get();
  ruby      = state_ruby(state);
  res       = rh_run_toplevel(ruby, "eruta_on_bump", "iii", ti1, ti2, kind);
  (void) data;
  return rh_tobool(res);
}


/* Collision handler for hulls. Calls an mruby callback. */
int callrb_collide_hulls(BumpHull * h1, BumpHull * h2, int kind, void * data) { 
  mrb_value res;
  State * state;
  int hi1, hi2, ti1, ti2;
  Ruby * ruby;
  hi1       = bumphull_id(h1);
  hi2       = bumphull_id(h2);
  ti1       = thing_id(bumphull_body_data(h1));
  ti2       = thing_id(bumphull_body_data(h2));
  state     = state_get();
  ruby      = state_ruby(state);
  res       = rh_run_toplevel(ruby, "eruta_on_bump", "iiiii", ti1, ti2, hi1, hi2, kind);
  (void) data;
  return rh_tobool(res);
}

/* Sprite event handler. Calls an mruby callback. */
int callrb_sprite_event(SpriteState * spritestate, int kind, void * data) { 
  mrb_value res;
  Sprite * sprite;
  State * state;
  Thing * thing;
  int spriteid, thingid, pose, direction;
  Ruby * ruby;
  sprite    = spritestate_sprite(spritestate);
  spriteid  = sprite_id(sprite);
  thing     = spritestate_data(spritestate);  
  pose      = spritestate_pose(spritestate);
  direction = spritestate_direction(spritestate);
  thingid   = thing_id(thing);
  state     = state_get();
  ruby      = state_ruby(state);
  res       = rh_run_toplevel(ruby, "eruta_on_sprite", "iiiii", 
                spriteid, thingid, pose, direction, kind);
  (void) data;
  return rh_tobool(res);
}

/* Calls the eruta_on_start function  */ 
int callrb_on_start() { 
  mrb_value res;
  State * state = state_get();
  Ruby * ruby   = state_ruby(state);
  res           = rh_run_toplevel(ruby, "eruta_on_start", "");
  return rh_tobool(res);
}

/* Calls the eruta_on_reload function  */
int callrb_on_reload() { 
  mrb_value res;
  State * state = state_get();
  Ruby * ruby   = state_ruby(state);
  res           = rh_run_toplevel(ruby, "eruta_on_reload", "");
  return rh_tobool(res);
}


/* Calls the eruta_on_update function. */
int callrb_on_update(State * self) {
  mrb_value res;  
  mrb_value mval = mrb_float_value(state_ruby(self), state_frametime(self));
  res = rh_run_toplevel_args(state_ruby(self), "eruta_on_update", 1, &mval);
  return rh_tobool(res);
}

