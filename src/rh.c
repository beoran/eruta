	
#include "eruta.h"
#include "image.h"
#include "rh.h"
#include "fifi.h"
#include "widget.h"
#include "str.h"
#include "mem.h"
#include "state.h"

#include <string.h>

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
 *
 * z: String from null terminated C string      [char*]
 * s: String                                    [char*, int] 
 * Y: sYmbol from null terminated C string      [char*]
 * y: symbol                                    [char*, int] 
 * 
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
        
      case 'y':
        str = va_arg(list, const char*);
        i   = va_arg(list, int);
        val = mrb_symbol_value(mrb_intern(ruby, str, (size_t)i));
        break;
        
      case 'Y':
        str = va_arg(list, const char*);
        val = mrb_symbol_value(mrb_intern_cstr(ruby, str));
        break; 
        
      case 'i':
        i   = va_arg(list, int);
        val = mrb_fixnum_value(i);
        break;
        
      case 'f':
        d   = va_arg(list, double);
        val = mrb_float_value(ruby, d);
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


/* strdup isn't ANSI C, just posix... :p so we need our own version.*/
char *rh_strdup(const char *str) {
    char * res = malloc(strlen(str) + 1);
    if(res) { strcpy(res, str); }
    return res;
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

/* Error reporter function type  */
typedef int (ScriptReporter)(Script * script, int status, const char * msg);


/* Enhanced wrapper around mruby. */
struct Script_ { 
  mrb_state       * mrb;
  ScriptReporter  * reporter;
  void            * report_to;
  int               report_result;
};

Script * script_alloc() {
  return STRUCT_ALLOC(Script);
}

Script * script_done(Script * self) {
  if(!self) return NULL;
  mrb_close(self->mrb);
  self->mrb           = NULL;
  self->report_to     = NULL;
  self->reporter      = NULL;
  self->report_result = FALSE;
  return self;
}

Script * script_free(Script * self) {
  script_done(self);
  mem_free(self);
  return NULL;
}

Script * script_init(Script * self, ScriptReporter * report, void * report_to) {
  if (!self) return NULL;
  self->mrb       = mrb_open();
  self->reporter  = report;
  self->report_to = report_to;
  return self;
}

Script * script_new(ScriptReporter * report, void * report_to) { 
  return script_init(script_alloc(), report, report_to);
}


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
  return rh_strdup(mrb_string_value_cstr(self, &value));
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

int script_report_exception(Script * self, mrb_value v) {
  int res = 0;
  char * str;
  /* Report exceptions */
  str = rh_exceptionstring(self->mrb);
  if(str) {
    if ((self->reporter) && (self->report_to)) { 
      res = self->reporter(self, -1, str);
    } else {
      fprintf(stderr, "mruby error: %s\n", str);
      res = -1;
    }
    free(str);
    return res;
  }
  return res;
}

int script_report_result(Script * self, mrb_value v) {
  char * str;
  int res = 0;
  /* Report result value if it's not nil. */
  if (self->reporter && (!mrb_nil_p(v))) {
    str = rh_inspect_cstr(self->mrb, v);
    res = self->reporter(self, 0, str);
    return res;
  }
  return res;
}

int script_report(Script * self, mrb_value v) {
  int    res = 0;
  char * str;
  res = script_report_exception(self, v);
  if (!self->report_result) return res;
  /* Report result if needed. */
  res = script_report_result(self, v);
  return res;
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
  char * buf;
  int runres;
  buf = malloc(strlen(filename) + 256);   
  if(!buf) return -3;
  strcpy(buf, "script/");
  strncat(buf, filename, strlen(filename));      
  ALLEGRO_PATH * path = fifi_data_vpath(buf);
  if(!path) { 
    free(buf);
    return -3;
  }
  runres = rh_runfilenamereport(self, PATH_CSTR(path), report, extra);
  al_destroy_path(path);
  free(buf);
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
mrb_value rh_dofunctionreport(Ruby * self, 
                        RhReporter reporter, void * extra,
                        mrb_value rubyself,                        
                        const char * funcname,    
                        int argc, mrb_value * argv) {
  int res = 0;
  char * str;
  mrb_value v;
  mrb_sym symname = mrb_intern_cstr(self, funcname);
  int ai;
  if(!mrb_respond_to(self, rubyself, symname)) {
    return mrb_nil_value();
  }  
  
  ai = mrb_gc_arena_save(self);
  v = mrb_funcall_argv(self, rubyself, symname, argc, argv);
  res = rh_make_report(self, v, reporter, extra);
  mrb_gc_arena_restore(self, ai);
  return v;
}

/* Executes a ruby function with the toplevel self as the ruby's object.
Errors are reported to the reporter callback if it isn't NULL. */
mrb_value rh_dotopfunctionreport(Ruby * self, 
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

/* Error report to console, AND to stderr if available. */
int rh_errorreporter_console(int status, const char * msg, void * extra) {
  char buf[80]; 
  if (status != 0) { 
    snprintf(buf, 80, "Error %d:", status);
    bbconsole_puts((BBConsole *)extra, buf);
    fprintf(stderr, "Error %d:\n", status);
  } 
  bbconsole_puts((BBConsole *)extra, msg);
  fprintf(stderr, "%s\n", msg);
  return 0;
}

/* Error report, depending on State. */
int rh_errorreporter_state(int status, const char * msg, void * extra) {
  State * state         = state_get();
  BBConsole * console   = state_console(state);
  if(bbconsole_active(console)) {
    return rh_errorreporter_console(status, msg, console);
  }
  return rh_errorreporter_file(status, msg, stderr);
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
mrb_value rh_runfunction_console(BBConsole * console, Ruby * ruby, mrb_value rubyself, 
                           char * name, int argc, mrb_value * argv) {
  return rh_dofunctionreport(ruby, rh_errorreporter_console, console, 
                             rubyself, name, argc, argv);
}

/** Runs a toplevel function in the ruby interpreter, logging results and errors back to
 * the console.
 */
mrb_value 
rh_runtopfunction_console(BBConsole * console, Ruby * ruby, 
                           char * name, int argc, mrb_value * argv) {
  return rh_dotopfunctionreport(ruby, rh_errorreporter_console, console, 
                                name, argc, argv);
}


/** Runs a function in the ruby interpreter, with C arguments according to the 
 * given format string, logging results and errors back to
 * the reporter. The limit is 32 arguments.
 */
mrb_value 
rh_dofunctionargs_report_va(Ruby * self, 
                        RhReporter reporter, void * extra,
                        mrb_value rubyself,
                        const char * funcname,
                        char * format, va_list list) {
  mrb_value argv[32];
  int argc;
  argc = rh_args_va(self, argv, 32, format, list);
  if (argc < 0) return mrb_nil_value();
  return 
  rh_dofunctionreport(self, reporter, extra, rubyself, funcname, argc, argv);
}

/** Runs a function in the ruby interpreter, logging results and errors back to
 * the console.
 */
mrb_value 
rh_runfunctionargs_console(BBConsole * console, Ruby * ruby, mrb_value rubyself, 
                           char * name, char * format, ...) {
  mrb_value res; 
  va_list list;
  va_start(list, format);
  res = rh_dofunctionargs_report_va(ruby, rh_errorreporter_console, console, rubyself, name, format, list);
  va_end(list);
  return res;
}

/** Runs a function in the ruby interpreter, logging results and errors back to
 * the console.
 */
mrb_value 
rh_runtopfunctionargs_console(BBConsole * console, Ruby * ruby, 
                                  char * name, char * format, ...) {
  mrb_value res; 
  va_list list;
  va_start(list, format);
  res = rh_dofunctionargs_report_va(ruby, rh_errorreporter_console, console, mrb_top_self(ruby), name, format, list);
  va_end(list);
  return res;
}


/** Runs a function in the ruby interpreter, logging results and errors back to
 * the state console or stderr if it's not active.
 */
mrb_value 
rh_runfunctionargs(Ruby * ruby, mrb_value rubyself, 
                   char * name, char * format, ...) {
  mrb_value res; 
  va_list list;
  va_start(list, format);
  res = rh_dofunctionargs_report_va(ruby, rh_errorreporter_state, state_get(), 
                                    rubyself, name, format, list);
  va_end(list);
  return res;
}

/** Runs a function in the ruby interpreter, under the stoplevel self. 
 * This logs results and errors back to the state console or stderr 
 * if it's not active.
 */
mrb_value 
rh_runtopfunctionargs(Ruby * ruby, char * name, char * format, ...) {
  mrb_value res; 
  va_list list;
  va_start(list, format);
  res = rh_dofunctionargs_report_va(ruby, rh_errorreporter_state, state_get(),
                                    mrb_top_self(ruby), name, format, list);
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
mrb_value rh_simple_funcall(char * name, void * ruby) {
  int ai;  
  Ruby * mrb = (Ruby *) ruby;  
  mrb_value args[16];
  ai = mrb_gc_arena_save(mrb);
  fprintf(stderr, "GC Area: %d\n", ai);
  // if(ai> 99) exit(0);
  mrb_value v = mrb_funcall_argv(mrb, mrb_top_self(mrb), mrb_intern_cstr(mrb, name), 
                    0, args);
  if (mrb->exc) {
    if (!mrb_undef_p(v)) {        
      mrb_p(mrb, mrb_obj_value(mrb->exc));
    }
    return mrb_nil_value();
  }
  // mrb_gc_arena_restore(mrb, 0);
  return v;
}

/* Maybe wrap this too? 
mrb_value mrb_funcall_argv(mrb_state*, mrb_value, mrb_sym, int, mrb_value*);
*/

/* Converts and mrb_value to a C boolean as per the ruby interpretation. */
int rh_tobool(mrb_value v) {
  if (mrb_nil_p(v))             return FALSE;
  return mrb_bool(v);
}

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

#define RH_EVARGS_MAX 32


/* Returns the current number of the joystick, that is it's index in the allegro 
 * JS list. Too bad but this number may change. Returns -1 if not found; */
static int joystick_nr(ALLEGRO_JOYSTICK * stick) {
  int index;
  ALLEGRO_JOYSTICK * aid;
  for(index = 0 ; index < al_get_num_joysticks(); index ++) {
    aid = al_get_joystick(index);
    if (aid == stick) return index;
  }
  return -1;
}

/* Send the even to the ruby  main "on_poll"   */
int rh_poll_event(mrb_state * mrb, ALLEGRO_EVENT * event) {
  State * state = state_get();
  mrb_value event_args[RH_EVARGS_MAX];
  int nargs = 0;
  
  if (!event) return FALSE;
  switch (event->type) {
    case ALLEGRO_EVENT_JOYSTICK_AXIS:
      nargs = 
      rh_args(mrb, event_args, RH_EVARGS_MAX, "Yfiiif", "joystick_axis",
              event->any.timestamp,
              joystick_nr(event->joystick.id), event->joystick.stick, event->joystick.axis, event->joystick.pos);
    break;
    case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
      nargs = 
      rh_args(mrb, event_args, RH_EVARGS_MAX, "Yfiiif", "joystick_button_down", 
              event->any.timestamp,
              joystick_nr(event->joystick.id), event->joystick.button);
    break;
    case ALLEGRO_EVENT_JOYSTICK_BUTTON_UP:
      nargs = 
      rh_args(mrb, event_args, RH_EVARGS_MAX, "Yfiiif", "joystick_button_up", 
              event->any.timestamp,
              joystick_nr(event->joystick.id), event->joystick.button);
    break;
    case ALLEGRO_EVENT_JOYSTICK_CONFIGURATION:
      nargs = 
      rh_args(mrb, event_args, RH_EVARGS_MAX, "Yfiiif", "joystick_configuration", 
              event->any.timestamp);
    break;
    case ALLEGRO_EVENT_KEY_DOWN:
      nargs = 
      rh_args(mrb, event_args, RH_EVARGS_MAX, "Yfi", "key_down",
              event->any.timestamp,
              event->keyboard.keycode   );
    break;
    case ALLEGRO_EVENT_KEY_UP:
      nargs = 
      rh_args(mrb, event_args, RH_EVARGS_MAX, "Yfi", "key_up",
              event->any.timestamp,
              event->keyboard.keycode   );
    break;
    case ALLEGRO_EVENT_KEY_CHAR:
      nargs = 
      rh_args(mrb, event_args, RH_EVARGS_MAX, "Yfiiib", "key_char",
              event->any.timestamp,
              event->keyboard.keycode, event->keyboard.unichar,
              event->keyboard.modifiers, event->keyboard.repeat
             );
    break;
    
    case ALLEGRO_EVENT_MOUSE_AXES:
      nargs = 
      rh_args(mrb, event_args, RH_EVARGS_MAX, "Yfiiiiiiii", "mouse_axes",
              event->any.timestamp,
              event->mouse.x, event->mouse.y, event->mouse.z, event->mouse.w,
              event->mouse.dx, event->mouse.dy, event->mouse.dz, event->mouse.dw
             );
    break;

    case ALLEGRO_EVENT_MOUSE_WARPED:
      nargs = 
      rh_args(mrb, event_args, RH_EVARGS_MAX, "Yfiiiiiiii", "mouse_warped",
              event->any.timestamp,
              event->mouse.x, event->mouse.y, event->mouse.z, event->mouse.w,
              event->mouse.dx, event->mouse.dy, event->mouse.dz, event->mouse.dw
             );
    break;
    
    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
      nargs = 
      rh_args(mrb, event_args, RH_EVARGS_MAX, "Yfiiiii", "mouse_button_down",
              event->any.timestamp,
              event->mouse.x, event->mouse.y, event->mouse.z, event->mouse.w,
              event->mouse.button
             );
    break;
    
    case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
      nargs = 
      rh_args(mrb, event_args, RH_EVARGS_MAX, "Yfiiiii", "mouse_button_up",
              event->any.timestamp,
              event->mouse.x, event->mouse.y, event->mouse.z, event->mouse.w,
              event->mouse.button
             );
    break;
    
    case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
      nargs = 
      rh_args(mrb, event_args, RH_EVARGS_MAX, "Yfiiii", "mouse_enter_display",
              event->any.timestamp,
              event->mouse.x, event->mouse.y, event->mouse.z, event->mouse.w
             );
    break;
    
    case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
      nargs = 
      rh_args(mrb, event_args, RH_EVARGS_MAX, "Yfiiii", "mouse_leave_display",
              event->any.timestamp,
              event->mouse.x, event->mouse.y, event->mouse.z, event->mouse.w
             );
    break;
       
    
    default:
      nargs = rh_args(mrb, event_args, RH_EVARGS_MAX, "Yf", "unknown",
        event->any.timestamp
      );
    break;
  }  
  rh_runtopfunction_console(state_console(state), state_ruby(state), 
                                 "on_poll", nargs, event_args);
  return true;
}

/* Polls the event queue and while events are available, send them to the ruby 
 main "on_poll"   */
int rh_poll_events(mrb_state * mrb, ALLEGRO_EVENT_QUEUE * queue) {
  ALLEGRO_EVENT event;
  while(al_get_next_event(queue, &event)) { 
    rh_poll_event(mrb, &event);
  }
  return TRUE;
}


/* Tries to (re-)load the main ruby file, output to console. */
int rh_load_main() { 
  State * state = state_get();
  // Try to load the mainruby file.
  if(state_console(state)) { 
    return rh_runfilename_console(state_console(state), "main.rb", state_ruby(state));
  } else {
    return rh_runfilename_stderr("main.rb", state_ruby(state));
  }
}
  
/* Calls the on_start function  */ 
int rh_on_start() { 
  State * state = state_get();
  return rh_dostring_console(state_console(state), "on_start()", state_ruby(state));
}

/* Calls the on_reload function  */ 
int rh_on_reload() { 
  State * state = state_get();
  return rh_dostring_console(state_console(state), "on_reload()", state_ruby(state));
}






