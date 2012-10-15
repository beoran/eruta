/**
* toruby.c contains all functionality that Eruta exposes to 
* mruby. All functions are prefixed with tr_
*/


#include "toruby.h"
#include "rh.h"
#include "state.h"

/* Test method. */
static int tr_test(mrb_state * mrb, mrb_value self) {
  printf("Hello from a mruby test!\n");
  return 0;
}


/* Writes to console or to stdout if console is not available. */
static int tr_log(mrb_state * mrb, mrb_value self) {
  
  State   * state   = state_get();
  Console * console = state_console(state);
  mrb_value text    = mrb_str_new2(mrb, "");
  mrb_get_args(mrb, "s", &text);
  if(console) {
    console_puts(console, RSTRING_PTR(text));
  } else {
    puts(RSTRING_PTR(text));
  }
  return 0;
}

/** Initializes the functionality that Eruta exposes to lua. */
int tr_init(mrb_state * mrb) {
  // luaL_dostring(lua, "print 'Hello!' ");
  struct RClass *krn;
  krn = mrb_class_get(mrb, "Kernel");
  if(!krn) return -1;
  mrb_define_method(mrb, krn, "test", tr_test, ARGS_REQ(1));
  mrb_define_method(mrb, krn, "log" , tr_log , ARGS_REQ(1));
  return 0;
}








