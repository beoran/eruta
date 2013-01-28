#ifndef rh_H_INCLUDED
#define rh_H_INCLUDED

#include <mruby.h>
#include <mruby/data.h>
#include <mruby/compile.h>
#include <mruby/proc.h>
#include <mruby/string.h>

// shortcut typedef
typedef mrb_state Ruby;

#include "widget.h"

void toruby_Font_free(mrb_state * state , void * ptr );

extern struct mrb_data_type toruby_Font_type;

void toruby_Path_free(mrb_state * state , void * ptr );

extern struct mrb_data_type toruby_Path;

mrb_value tr_Path (Ruby * ruby , mrb_value self );

Ruby * rh_new(void);

Ruby * rh_free (Ruby * self );

mrb_value rh_inspect (mrb_state *mrb , mrb_value obj );

char * rh_inspect_cstr (mrb_state *mrb , mrb_value value );

int rh_runfile (Ruby * self , const char * filename , FILE * file );

int rh_runfilename (Ruby * self , const char * filename );

char * rh_exceptionstring (Ruby * self );

int rh_dofile(Ruby * self , const char * filename );

int rh_errorreporter_file (int status , const char * msg , void * extra );

int rh_errorreporter_console (int status , const char * msg , void * extra );

int rh_dostring_console (BBConsole * console , char * command , void * extra );


#endif




