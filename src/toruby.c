/**
* toruby.c contains all functionality that Eruta exposes to 
* mruby. All functions are prefixed with tr_
*/


#include "toruby.h"
#include "rh.h"
#include "state.h"

/* Test method. */
static mrb_value tr_test(mrb_state * mrb, mrb_value self) {
  printf("Hello from a mruby test!\n");
  return self;
}


/* Writes to console or to stdout if console is not available. */
static mrb_value tr_log(mrb_state * mrb, mrb_value self) {
  
  State   * state   = NULL;
  Console * console = NULL;
  state             = state_get();
  if (state) {
    console = state_console(state);
  } else {
    puts("No state?!");
  }
  mrb_value text    = mrb_nil_value();
  mrb_get_args(mrb, "S", &text);
  if(console) {
    console_puts(console, RSTRING_PTR(text));
  } else {
    puts(RSTRING_PTR(text));
  }
  return self;
}

/** Initializes the functionality that Eruta exposes to lua. */
int tr_init(mrb_state * mrb) {
  // luaL_dostring(lua, "print 'Hello!' ");
  struct RClass *krn;
  krn = mrb_class_get(mrb, "Kernel");
  if(!krn) return -1;
  mrb_define_method(mrb, krn, "test", 
                    tr_test, ARGS_REQ(1));
  mrb_define_method(mrb, krn, "log" , tr_log , ARGS_REQ(1));
  return 0;
}








