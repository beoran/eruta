

#include "collide.h"
#include "state.h"
#include "rh.h"


/* Collision handler. Calls an mr*/
int collide_things(Thing * t1, Thing * t2, int kind, void * data) { 
  int res;
  BBConsole * console;
  State * state;
  int ti1, ti2;
  Ruby * ruby;
  ti1       = thing_id(t1);
  ti2       = thing_id(t2);
  state     = state_get();
  ruby      = state_ruby(state);
  console   = state_console(state);

  res = rh_runtopfunctionargs_console(console, ruby, "on_bump", "iii", ti1, ti2, kind);

  return TRUE;
}




