
#include "eruta.h"
#include "image.h"
#include "rh.h"
#include "fifi.h"
#include "widget.h"
#include "str.h"

#include <stdarg.h>
#include <mruby.h>

/*
* RH contains helper functions for the mruby ruby interpreter.
*/

/*
 * Converts C like arguments to an array of mruby mrb_values.
 * Returns amount of arguments parsed, negative on failure.
 * 
 * Format specifier characters for the format argument: 
 * z: String from null terminated C string      [char*]
 * s: String                                    [char*, int] 
 * f: Float                                     [double]
 * i: Integer                                   [int]
 * b: Boolean                                   [int] 
 * 0: nil
 * Other characters: Error. Will return -(index or erroneous character) .
 */ 
int rh_args_va(Ruby * ruby, mrb_value * values,  int size,  char * format, va_list list) {
  int ch; int index;
  int i; double d; const char * str;
  index = 0; 
  
  for (ch = (*format) ; (ch) && (index < size) ; format++ , ch = (*format)) {
    mrb_value val;
    int error = FALSE;
    switch(ch) {
      case 's':
        str = va_arg(list, const char*);
        i   = va_arg(list, int);
        val = mrb_str_new(ruby, str, i);
        break;
        
      case 'z':
        str = va_arg(list, const char*);
        val = mrb_str_new_cstr(ruby, str);
        break;
        
      case 'i':
        i   = va_arg(list, int);
        val = mrb_fixnum_value(i);
        break;
        
      case 'f':
        d   = va_arg(list, double);
        val = mrb_float_value(d);
        break;
        
      case 'b': 
        i   = va_arg(list, int);
        val = ( i ? mrb_true_value() : mrb_false_value());
        break;
      
      case '0':
        val = mrb_nil_value();
        break;
        
      default:
        error = TRUE;
        break;
    }
    if (error) { 
      return -index;
    }
    values[index] = val;
    index ++;
  }
  return index;
}

/* Helps convert C values to mruby values in an array. */
int rh_args(Ruby * ruby, mrb_value * values,  int size,  char * format, ...) {
  int res;
  va_list list;
  va_start(list, format);
  res = rh_args_va(ruby, values, size, format, list);
  va_end(list);
  return res;
}




/* Error reporter function type  */
typedef int (RhReporter)(int status, const char * msg, void * extra);

/** Calulates the execption string. Result only tempoarily available..
XXX: check if this doesn't leak memory... you must free the results manually.
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
   /*mrb_define_method(self, self->kernel_module, 
                     "path", tr_Path, ARGS_REQ(1));*/
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
  return mrb_inspect(mrb, obj);
}

char * rh_inspect_cstr(mrb_state *mrb, mrb_value value) {
  mrb_value res = rh_inspect(mrb, value);
  return mrb_string_value_cstr(mrb, &res);
}

/* Does the actual reporting  depending on the current state of 
 ruby and the returned value. */
int rh_make_report(Ruby * self, mrb_value v, RhReporter * reporter, void * extra) { 
  int res = 0;
  char * str;
 
  /* Report exceptions */
  str = rh_exceptionstring(self);
  if(str) {
    if(reporter) { 
      res = reporter(-1, str, extra);
    } else  {
      fprintf(stderr, "mruby error: %s\n", str);
      res = -1;
    }
    free(str);
    return res;
  }
  
  /* Report result value if it's not nil. */
  if (reporter && (!mrb_nil_p(v))) {
    str = rh_inspect_cstr(self, v);
    res = reporter(0, str, extra);
    return res;
  }
  return res;
}

/* Runs a file and reports any errors over the reporter calback if it isn't
NULL. */
int rh_runfilereport(Ruby * self, const char * filename, FILE * file, 
                      RhReporter reporter, void * extra) {
  int res;
  char * str;
  mrbc_context * c ; 
  mrb_value v;
  int ai;
  ai = mrb_gc_arena_save(self);
  c = mrbc_context_new(self);  
  mrbc_filename(self, c, filename);
  v = mrb_load_file_cxt(self, file, c);
  mrbc_context_free(self, c);  
  /* Report exceptions */
  res = rh_make_report(self, v, reporter, extra);  
  mrb_gc_arena_restore(self, ai);
  return res;
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
  int res = 0;
  char * str;
  mrb_value v;
  int ai;
  ai = mrb_gc_arena_save(self);  
  #ifdef RH_USE_CONTEXT
  mrbc_context      * c  = mrbc_context_new(self);
  mrbc_filename(self, c, "command");
  v = mrb_load_string_cxt(self, command, c);
  mrbc_context_free(self, c); 
  #else
  v = mrb_load_string(self, command);
  #endif
  /* Report exceptions */
  res = rh_make_report(self, v, reporter, extra);
  /* mrb GC area seems to be an area for 1024 "new" objects for the generational 
   * GC. It can overflow if a lot of new objects are generated 
   * (say exceptions, etc) on the C side. To prevent this the area must be saved 
   * and restored anywhere many ruby objects may have been generated.
   * It seems that here too this is needed.
   */
  mrb_gc_arena_restore(self, ai);
  return res;
}


/* Executes a ruby function with parameters. 
Errors are reported to the reporter callback if it isn't NULL. */
int rh_dofunctionreport(Ruby * self, 
                        RhReporter reporter, void * extra,
                        mrb_value rubyself,                        
                        const char * funcname,    
                        int argc, mrb_value * argv) {
  int res = 0;
  char * str;
  mrb_value v;
  int ai;
  ai = mrb_gc_arena_save(self);
  
  v = mrb_funcall_argv(self, rubyself, mrb_intern(self, funcname), argc, argv);
  res = rh_make_report(self, v, reporter, extra);
  mrb_gc_arena_restore(self, ai);
  return res;
}

/* Executes a ruby function with the toplevel self as the ruby's object.
Errors are reported to the reporter callback if it isn't NULL. */
int rh_dotopfunctionreport(Ruby * self, 
                        RhReporter reporter, void * extra,
                        const char * funcname, 
                        int argc, mrb_value * argv) {
  mrb_value top = mrb_top_self(self);
  return rh_dofunctionreport(self, reporter, extra, top, funcname, argc, argv);
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

/** Runs a function in the ruby interpreter, logging results and errors back to
 * the console.
 */
int rh_runfunction_console(BBConsole * console, Ruby * ruby, mrb_value rubyself, 
                           char * name, int argc, mrb_value * argv) {
  return rh_dofunctionreport(ruby, rh_errorreporter_console, console, 
                             rubyself, name, argc, argv);
}

/** Runs a toplevel function in the ruby interpreter, logging results and errors back to
 * the console.
 */
int rh_runtopfunction_console(BBConsole * console, Ruby * ruby, 
                           char * name, int argc, mrb_value * argv) {
  return rh_dotopfunctionreport(ruby, rh_errorreporter_console, console, 
                                name, argc, argv);
}


/** Runs a function in the ruby interpreter, with C arguments according to the 
 * given format string, logging results and errors back to
 * the reporte. The limit is 32 arguments.
 */
int 
rh_dofunctionargs_report_va(Ruby * self, 
                        RhReporter reporter, void * extra,
                        mrb_value rubyself,
                        const char * funcname,
                        char * format, va_list list) {
  mrb_value argv[32];
  int argc;
  argc = rh_args_va(self, argv, 32, format, list);
  if (argc < 0) return argc;
  return 
  rh_dofunctionreport(self, reporter, extra, rubyself, funcname, argc, argv);
}

/** Runs a function in the ruby interpreter, logging results and errors back to
 * the console.
 */
int rh_runfunctionargs_console(BBConsole * console, Ruby * ruby, mrb_value rubyself, 
                           char * name, char * format, ...) {
  int res; 
  va_list list;
  va_start(list, format);
  res = rh_dofunctionargs_report_va(ruby, rh_errorreporter_console, console, rubyself, name, format, list);
  va_end(list);
  return res;
}

/** Runs a function in the ruby interpreter, logging results and errors back to
 * the console.
 */
int rh_runtopfunctionargs_console(BBConsole * console, Ruby * ruby, 
                                  char * name, char * format, ...) {
  int res; 
  va_list list;
  va_start(list, format);
  res = rh_dofunctionargs_report_va(ruby, rh_errorreporter_console, console, mrb_top_self(ruby), name, format, list);
  va_end(list);
  return res;
}




/** Runs a ruby string and logs results and errors to stderr.
*/
int rh_dostring_stderr(char * command, void * extra) {
  Ruby * ruby = (Ruby *) extra;
  return rh_dostringreport(ruby, command, rh_errorreporter_file, stderr);
}

/** Runs a file in the ruby interpreter, logging results and errors back to
 * the console.
 */
int rh_runfilename_stderr(char * name, void * extra) {
  Ruby * ruby = (Ruby *) extra;
  return rh_dofilereport(ruby, name, rh_errorreporter_file, stderr);
}



/* Calls a function, doesn't log anything. */
int rh_simple_funcall(char * name, void * ruby) {
  int ai;  
  Ruby * mrb = (Ruby *) ruby;  
  mrb_value args[16];
  ai = mrb_gc_arena_save(mrb);
  fprintf(stderr, "GC Area: %d\n", ai);
  // if(ai> 99) exit(0);
  mrb_value v = mrb_funcall_argv(mrb, mrb_top_self(mrb), mrb_intern(mrb, name), 
                    0, args);
  if (mrb->exc) {
    if (!mrb_undef_p(v)) {        
      mrb_p(mrb, mrb_obj_value(mrb->exc));
    }
    return -1;
  }
  // mrb_gc_arena_restore(mrb, 0);
  return 0;
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









