

#include "collide.h"
#include "state.h"
#include "rh.h"


/* Collision handler for things. Calls an mruby callback. */
int collide_things(Thing * t1, Thing * t2, int kind, void * data) { 
  mrb_value res;
  BBConsole * console;
  State * state;
  int ti1, ti2;
  Ruby * ruby;
  ti1       = thing_id(t1);
  ti2       = thing_id(t2);
  state     = state_get();
  ruby      = state_ruby(state);
  res       = rh_run_toplevel(ruby, "on_bump", "iii", ti1, ti2, kind);
  (void) data;
  return rh_tobool(res);
}


/* Collision handler for hulls. Calls an mruby callback. */
int collide_hulls(BumpHull * h1, BumpHull * h2, int kind, void * data) { 
  mrb_value res;
  BBConsole * console;
  State * state;
  int hi1, hi2, ti1, ti2;
  Ruby * ruby;
  hi1       = bumphull_id(h1);
  hi2       = bumphull_id(h2);
  ti1       = thing_id(bumphull_body_data(h1));
  ti2       = thing_id(bumphull_body_data(h2));
  state     = state_get();
  ruby      = state_ruby(state);
  res       = rh_run_toplevel(ruby, "on_bump", "iiiii", ti1, ti2, hi1, hi2, kind);
  (void) data;
  return rh_tobool(res);
}




