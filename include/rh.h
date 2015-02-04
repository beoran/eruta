#ifndef rh_H_INCLUDED
#define rh_H_INCLUDED

#include <mruby.h>
#include <mruby/data.h>
#include <mruby/compile.h>
#include <mruby/proc.h>
#include <mruby/string.h>

// shortcut typedef. Also to alllow possible future enhancement.
typedef mrb_state Ruby;

typedef struct Script_ Script;


#include "widget.h"

void toruby_Font_free(mrb_state * state , void * ptr );

extern struct mrb_data_type toruby_Font_type;

void toruby_Path_free(mrb_state * state , void * ptr );

extern struct mrb_data_type toruby_Path;

mrb_value tr_Path (Ruby * ruby, mrb_value self,  struct RClass * klass); 

Ruby * rh_new(void);

Ruby * rh_free(Ruby * self );

mrb_value rh_inspect(mrb_state *mrb , mrb_value obj );

char * rh_inspect_cstr(mrb_state *mrb , mrb_value value);

int rh_run_file (Ruby * self , const char * filename, FILE * file );

int rh_run_filename (Ruby * self , const char * filename );

/* rh_run_script only works fo files in a (sub) folder of the Eruta script
 * directory, where rh_run_filename is generic. */
int rh_run_script(Ruby * self, const char * filename);

char * rh_exception_string (Ruby * self );

mrb_value rh_simple_funcall(Ruby * ruby, char * name);

mrb_value rh_run_function_args(Ruby * ruby, mrb_value rubyself, 
                          char * name, int argc, mrb_value * argv);

mrb_value rh_run_toplevel_args(Ruby * ruby, char * name, int argc, mrb_value * argv);

mrb_value rh_run_function_va(Ruby * ruby, mrb_value rubyself, 
                          char * name, char * format, va_list args);

mrb_value rh_run_toplevel_va(Ruby * ruby, char * name, char * format, va_list args);


mrb_value rh_run_function(Ruby * ruby, mrb_value rubyself, char * name, char * format, ...);

mrb_value rh_run_toplevel(Ruby * ruby, char * name, char * format, ...);
int rh_tobool(mrb_value v);                              
                              
#define rh_bool_value(B) ( (B) ? mrb_true_value() : mrb_false_value())

int rh_poll_event(mrb_state * mrb, ALLEGRO_EVENT * event);

int rh_poll_events(mrb_state * mrb, ALLEGRO_EVENT_QUEUE * queue);

int rh_load_main();
int rh_on_start();
int rh_on_reload(); 

int rh_run_console_command(BBConsole * console, const char * command, void * extra);

#endif




