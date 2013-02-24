
#include "eruta.h"
#include "image.h"
#include "rh.h"
#include "fifi.h"
#include "widget.h"
#include "str.h"



/*
* RH contains helper functions for the mruby ruby interpreter.
*/

/* Error reporter function type  */
typedef int (RhReporter)(int status, const char * msg, void * extra);

/** Calulates the execption string. Reult only tempoarily available..
XXX: check if this doesn't leak memory...
*/
char * rh_exceptionstring(Ruby * self) {
  char      * result;
  mrb_value   value;
  if(!self->exc) return NULL; 
  // 
  value  = mrb_funcall(self, mrb_obj_value(self->exc), "inspect", 0);
  // reset exception since we have it's string value.
  // Does this leak memory or not???
  self->exc = NULL;
  return strdup(mrb_string_value_cstr(self, &value));
}


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


/* Runs a file and reports any errors over the reporter calback if it isn't
NULL. */
int rh_runfilereport(Ruby * self, const char * filename, FILE * file, 
                      RhReporter reporter, void * extra) {
  int res;
  char * str;
  mrbc_context * c  = mrbc_context_new(self);
  mrb_value v;
  
  mrbc_filename(self, c, filename);
  v = mrb_load_file_cxt(self, file, c);
  mrbc_context_free(self, c);
  
  /* Report exceptions */
  str = rh_exceptionstring(self);
  if(str) {
    if(reporter) { 
      res = reporter(-1, str, extra);
    } else  {
      mrb_p(self, mrb_obj_value(self->exc));
      res = -1;
    }
    free(str);
    return res;
  }
  
  /* Report result value. */
  if (reporter) {
    str = rh_inspect_cstr(self, v);
    return reporter(0, str, extra);
  }
  return 0;
}


/** Runs a file without error reporting. */
int rh_runfile(Ruby * self, const char * filename, FILE * file) {
  return rh_runfilereport(self, filename, file, NULL, NULL);
}

/** Runs a named file with reporting. */
int rh_runfilenamereport(Ruby * self, const char * filename,
                         RhReporter * reporter, void * extra) {
  FILE * file = fopen(filename, "rt");
  int res;
  if(!file) { 
    int res;
    USTR * estr = ustr_newf("No such ruby file: %s\n", filename);
    if (reporter) { 
      res = reporter(-2, ustr_c(estr), extra);
    } else {
      res = -2;
      fprintf(stderr, "%s", ustr_c(estr));
    }
    ustr_free(estr);
    return res;
  }
  res = rh_runfile(self, filename, file);
  fclose(file);
  return res;
}


/** Runs a named file. */
int rh_runfilename(Ruby * self, const char * filename) {
  return rh_runfilenamereport(self, filename, NULL, NULL);
}

/**
* Executes a ruby file in Eruta's data/script directory with reporting.
* Returns -2 if the file was not found.
* Returns -3 if the path wasn't found.
*/
int rh_dofilereport(Ruby * self, const char * filename, 
              RhReporter * report, void * extra) {
  int runres;
  ALLEGRO_PATH * path = fifi_data_pathargs(filename, "script", NULL);
  if(!path) return -3;
  runres = rh_runfilenamereport(self, PATH_CSTR(path), report, extra);
  al_destroy_path(path);
  return runres;
}

/**
* Executes a ruby file in Eruta's data/script directory.
* Returns -2 if the file was not found.
* Returns -3 if the path wasn't found.
*/
int rh_dofile(Ruby * self, const char * filename) {
  return rh_dofilereport(self, filename, NULL, NULL);
}


/* Executes a ruby command string. 
Errors are reported to the reporter callback if it isn't NULL. */
int rh_dostringreport(Ruby * self, const char * command, 
                      RhReporter reporter, void * extra) {
  int res;
  char * str;
  mrb_value v;
  #ifdef RH_USE_CONTEXT
  mrbc_context      * c  = mrbc_context_new(self);
  mrbc_filename(self, c, "command");
  v = mrb_load_string_cxt(self, command, c);
  mrbc_context_free(self, c); 
  #else
  v = mrb_load_string(self, command);
  #endif
  
  /* v = mrb_load_string(self, command); is also possible but 
    doesn't show  file information. */
  str = rh_exceptionstring(self);
  if(str) {
    if(reporter) { 
      res = reporter(-1, str, extra);
    } else  {
      mrb_p(self, mrb_obj_value(self->exc));
      res = -1;
    }
    free(str);
    return res;
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
  if (status != 0) { 
    snprintf(buf, 80, "Error %d:", status);
    bbconsole_puts((BBConsole *)extra, buf);
  } 
  bbconsole_puts((BBConsole *)extra, msg);
  return 0;
}


/** Runs a ruby string in the console, logging results and errors back to it.
*/
int rh_dostring_console(BBConsole * console, char * command, void * extra) {
  Ruby * ruby = (Ruby *) extra;
  return rh_dostringreport(ruby, command, rh_errorreporter_console, console);
}

/** Runs a file in the ruby interpreter, logging results and errors back to
 * the console.
 */
int rh_runfilename_console(BBConsole * console, char * name, void * extra) {
  Ruby * ruby = (Ruby *) extra;
  return rh_dofilereport(ruby, name, rh_errorreporter_console, console);
}

/* Maybe wrap this too? 
mrb_value mrb_funcall_argv(mrb_state*, mrb_value, mrb_sym, int, mrb_value*);
*/


/* Documentation for mrb_get_args
  retrieve arguments from mrb_state.

  mrb_get_args(mrb, format, ...)
  
  returns number of arguments parsed.

  format specifiers:

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









