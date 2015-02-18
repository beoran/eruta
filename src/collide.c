

#include "collide.h"
#include "state.h"
#include "rh.h"


/* Collision handler. Calls an mruby callback. */
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
  console   = state_console(state);
  res = rh_run_toplevel(ruby, "on_bump", "iii", ti1, ti2, kind);
  (void) data;
  return rh_tobool(res);
}




