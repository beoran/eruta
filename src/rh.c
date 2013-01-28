
#include "eruta.h"
#include "image.h"
#include "rh.h"
#include "fifi.h"
#include "widget.h"



/*
* RH contains helper functions for the mruby ruby interpreter.
*/


/** Allocates and initialzes a new ruby state. */
Ruby * rh_new() {
   Ruby * self = mrb_open();
   mrb_define_method(self, self->kernel_module, 
                     "path", tr_Path, ARGS_REQ(1));
   return self;
}

/** Frees a ruby state. */
Ruby * rh_free(Ruby * self) {
  mrb_close(self);
  return NULL;
}

/** Returns an mrb_value that contains the value of object.inspect. 
* m
*/
mrb_value rh_inspect(mrb_state *mrb, mrb_value obj) {
  return mrb_funcall(mrb, obj, "inspect", 0);
}

char * rh_inspect_cstr(mrb_state *mrb, mrb_value value) {
  mrb_value res = rh_inspect(mrb, value);
  return mrb_string_value_cstr(mrb, &res);
}


int rh_runfile(Ruby * self, const char * filename, FILE * file) {
  mrbc_context     * c  = mrbc_context_new(self);
  mrb_value v;
  mrbc_filename(self, c, filename);
  v = mrb_load_file_cxt(self, file, c);

  mrbc_context_free(self, c);
  if (self->exc) {
      if (!mrb_undef_p(v)) {
        mrb_p(self, mrb_obj_value(self->exc));
      }
      return -1;
  }
  return 0;
}

int rh_runfilename(Ruby * self, const char * filename) {
  FILE * file = fopen(filename, "rt");
  int res;
  if(!file) { 
    printf("No such ruby file: %s\n", filename);
    return -2;
  }
  res = rh_runfile(self, filename, file);
  fclose(file);
  return res;
}

char * rh_exceptionstring(Ruby * self) {
  char      * result;
  mrb_value   value;
  if(!self->exc) return NULL; 
  // 
  value  = mrb_funcall(self, mrb_obj_value(self->exc), "inspect", 0);
  // reset exception since we have it's string value.
  // Does this leak memory or not???
  self->exc = NULL;
  return mrb_string_value_cstr(self, &value);
}


/**
* Executes a ruby file in Eruta's data/script directory.
* Returns -2 if the file was not found.
*/
int rh_dofile(Ruby * self, const char * filename) {
  int runres;
  ALLEGRO_PATH * path = fifi_data_pathargs(filename, "script", NULL);
  if(!path) return -1;
  runres = rh_runfilename(self, PATH_CSTR(path));
  al_destroy_path(path);
  return runres;
}

/* Error reporter function type  */
typedef int (RhErrorReporter)(int status, const char * msg, void * extra);

/* Executes a ruby command string. 
Errors are reported to the reporter callback if it isn't NULL. */
int rh_dostringreport(Ruby * self, const char * command, 
                      RhErrorReporter reporter, void * extra) {
  const char * str;
  mrbc_context     * c  = mrbc_context_new(self);
  mrb_value v;
  mrbc_filename(self, c, "command");
  v = mrb_load_string_cxt(self, command, c);

  mrbc_context_free(self, c);
  str = rh_exceptionstring(self);
  if(str) {
    if(reporter) { 
      return reporter(-1, str, extra);
    } else  {
      mrb_p(self, mrb_obj_value(self->exc));
      return -1;
    }
  }
  if (reporter) {
    str = rh_inspect_cstr(self, v);
    return reporter(0, str, extra);
  }
  return 0;
}


/* Error report to a file */
int rh_errorreporter_file(int status, const char * msg, void * extra) {
  return fprintf((FILE *) extra, "Error %d: %s\n", status, msg);
}

/* Error report to console */
int rh_errorreporter_console(int status, const char * msg, void * extra) {
  char buf[80]; 
  snprintf(buf, 80, "Error %d:", status);
  bbconsole_puts((BBConsole *)extra, buf);
  bbconsole_puts((BBConsole *)extra, msg);
  return 0;
}


/** Runs a ruby string in the console, logging results and errors back to it.
*/
int rh_dostring_console(BBConsole * console, char * command, void * extra) {
  Ruby * ruby = (Ruby *) extra;
  return rh_dostringreport(ruby, command, rh_errorreporter_console, console);
}



/* Documentation for mrb_get_args
  retrieve arguments from mrb_state.

  mrb_get_args(mrb, format, ...)
  
  returns number of arguments parsed.

  fortmat specifiers:

   o: Object [mrb_value]
   S: String [mrb_value]
   A: Array [mrb_value]
   H: Hash [mrb_value]
   s: String [char*,int]
   z: String [char*]
   a: Array [mrb_value*,int]
   f: Float [mrb_float]
   i: Integer [mrb_int]
   n: Symbol [mrb_sym]
   &: Block [mrb_value]
   *: rest argument [mrb_value*,int]
   |: optional
 */









