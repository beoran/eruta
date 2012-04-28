/** Lua helper functions. */
#include <stdlib.h>
#include <string.h>
#include <lauxlib.h>
#include "eruta.h"
#include "str.h"
#include "lh.h"
#include "fifi.h"
// needed for console output for lua errors.
#include "widget.h"


/* Not declared in lauxlib.h somehow, but it exists... */
void luaL_openlibs (Lua *L);

/** Lua helper functions and macros. */
Lua * lh_new() {
  Lua * lua = luaL_newstate();
  if(!lua) {
    return NULL;
  }
  luaL_openlibs(lua);
  return lua;
}

/** Frees a lua state if it's non-NULL. */
Lua * lh_free(Lua * lua) {
  if(lua) lua_close(lua);
  return NULL;
}


/** Creates lua user data in which the pointer to data will be stored.
* You must set up the metatable first thoug lh_datainit and lh_datamethod
*/
void ** lh_pushdata(Lua *L, const char * name, void * data) {
  void ** ptr = lua_newuserdata(L, sizeof(void*));
  (*ptr)      = data;
  luaL_getmetatable(L, name);
  lua_setmetatable(L, -2);
  return ptr;
}

/** Gets the object at stack index index as a pointer to data */
void * lh_todata(Lua *L,  int index) {
  void **ptr = (void **)lua_touserdata(L, index);
  printf("lh_todata %p\n", ptr);
  if (!ptr) { 
    fprintf(stderr, "Argument %i is NULL.\n", index);
    luaL_error(L, "Argument %i is NULL.\n", index);
    return NULL;    
  }  
  return *ptr;
}

/** Gets the object at stack index index as a pointer to data. 
Performs type checking. */
void * lh_checkdata(Lua *L, const char * name, int index) {
  void *data;
  luaL_checkudata (L, index, name);
  // 
  data = lh_todata(L, index);  
  if (!data) { 
    fprintf(stderr, "Data for %s is NULL.", name);
    luaL_error(L, "Data may not be NULL");
    return NULL;
  }  
  return data;
}

/** Gets the object on the top of the stack (first argument). Useful
for OOP-style methods or __gc metamethods. */
void * lh_getself(Lua *L, const char * name) {
  // int nargs = lua_gettop(L);
  return lh_checkdata(L, name, 1);
}

/** Makes it easier to parse the arguments a Lua function has received.
* Works in the spirit of scanf(), but with different specifiers.
* d : double *, i: int *, l : long *, s : char **, S strdupped char **, p
* userdata pointer. Retuns amount of arguments detected.
*/
int lh_scanargs_va(Lua *L, char * format, va_list args) {
  char * c;
  int index = 1;
  int nargs = lua_gettop(L);  
  for (c = format; c && (*c) != '\0' && (index <= nargs); c++, index++) {
    char t = (*c);
    switch(t) {
      case 'd':  { 
          double * d;
          d = va_arg(args, double*);
          if (d) { (*d) = luaL_checknumber(L, index); }
      }
      break;
      case 'i': { 
          int * i;
          i = va_arg(args, int*);
          if (i) { (*i) = luaL_checkint(L, index); } 
      }
      break;
      case 'l': { 
        long * l;
        l = va_arg(args, long*);
        if (l) { (*l) = luaL_checkint(L, index); }
      }
      break;
      case 's': {
        char ** s;
        s = va_arg(args, char**);
        if (s) { (*s) = (char *) luaL_checkstring(L, index); }
      }
      break;
      case 'D': {
        char ** s;
        s = va_arg(args, char**);
        if (s) { (*s) = strdup(luaL_checkstring(L, index)); }
      }
      break;
      case 'S': {
        STR ** s;
        s = va_arg(args, STR**);
        if (s) { (*s) = str_new(luaL_checkstring(L, index)); }
      }
      break;
      case 'p': {
        void ** p; 
        p = va_arg(args, void**);
        if(p) { (*p) = lh_todata(L, index); }
      }
      break;
      default:
      // unknown does nothing.
      break;
    } 
  }    
  return nargs;
};


/** Makes it easier to parse the arguments a Lua function has received.
* Works in the spirit of scanf(), but with different specifiers.
* d : double *, i: int *, l : long *, s : char **, D strDupped into char **,
* S : STR ** (Allegro string),  p: userdata pointer.
* Retuns amount of arguments detected.
*/
int lh_scanargs(Lua *L, char * format, ...) {
  va_list args;
  int result;
  va_start(args, format);
  result = lh_scanargs_va(L, format, args);
  va_end(args);
  return result;
}  


/* error reporter function type  */
typedef int (LhErrorReporter)(int status, const char * msg, void * extra);

/* error report to a file */
int lh_errorreporter_file(int status, const char * msg, void * extra) {
  return fprintf((FILE *) extra, "Error %d: %s\n", status, msg);
}

/* error report to console */
int lh_errorreporter_console(int status, const char * msg, void * extra) {
  char buf[80]; 
  snprintf(buf, 80, "Error %d:", status);
  console_puts((Console *)extra, buf);
  console_puts((Console *)extra, msg);
  return 0;
}


/* report errors generically  */
int lh_report_generic(Lua *L, int status, 
                      LhErrorReporter * reporter, void *extra) {
  if (status != LUA_OK && !lua_isnil(L, -1)) {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(error object is not a string)";
    reporter(status, msg, extra);
    lua_pop(L, 1);
    /* force a complete garbage collection in case of errors */
    lua_gc(L, LUA_GCCOLLECT, 0);
  }
  return status;
}


/** report errors to console  */
int lh_report_console(Lua *L, int status, void * console) {
  return lh_report_generic(L, status, lh_errorreporter_console, console);
}



/** report errors to stderr  */
int lh_report_stderr(Lua *L, int status) {
  return lh_report_generic(L, status, lh_errorreporter_file, stderr);
}

/* traceback function*/
int lh_traceback (Lua *L) {
  const char *msg = lua_tostring(L, 1);
  if (msg) {  luaL_traceback(L, L, msg, 1); }
  else if (!lua_isnoneornil(L, 1)) {  /* is there an error object? */
  if (!luaL_callmeta(L, 1, "__tostring"))  /* try its 'tostring' metamethod */
    lua_pushliteral(L, "(no error message)");
  }
  return 1;
}

/* Like lua_pcall, but generates a traceback. */
int lh_tracecall(Lua *L, int narg, int nres) {
  int status;
  int base = lua_gettop(L) - narg;  /* function index */
  lua_pushcfunction(L, lh_traceback);  /* push traceback function */
  lua_insert(L, base);  /* put it under chunk and args */
  status = lua_pcall(L, narg, nres, base);
  lua_remove(L, base);  /* remove traceback function */
  return status;
}

/* function pointer type for loading omething and makeing a trace on error*/
typedef int (LhLoadSomething)(Lua *L, const char * str);


/*
* Executes something after loading it
* Returns -1 if the name was not found according to LhLoadSomething.
*/
int lh_dosomething(Lua *L, const char * name, LhLoadSomething * loader) {
  int runres, loadres;  
  if(!name) return -1;
  loadres = loader(L, name);
  if(loadres) return loadres;
  runres  = lh_tracecall(L, 0, LUA_MULTRET);
  return runres;
}

int lh_loadfile(Lua *L, const char * name) {
  return luaL_loadfile(L, name);
}

int lh_getglobal(Lua *L, const char  * name) {
  lua_getglobal(L, name);
  return 0;
}


/**
* Executes a file in Eruta's data/script directory.
* Returns -1 if the file ws not found.
*/
int lh_dofile(Lua *L, const char * filename) {
  int runres;
  ALLEGRO_PATH * path = fifi_data_pathargs(filename, "script", NULL);
  if(!path) return -1;
  runres = lh_dosomething(L, PATH_CSTR(path), lh_loadfile);
  al_destroy_path(path);
  return runres;
}

/** Executes a string in the lua interpreter and handles errors
* by putting atraceback on the stack
*
*/
int lh_dostring(Lua *L, const char * dorun) {
  int runres;
  if(!dorun) return -1;
  runres = lh_dosomething(L, dorun, luaL_loadstring);
  return runres;
}


/** Executes then named global function in the lua interpreter and 
* handles errors by putting a traceback on the stack
*
*/
int lh_dofunction(Lua *L, const char * funcname) {
  int runres;
  if(!funcname) return -1;
  runres = lh_dosomething(L,  funcname, lh_getglobal);
  return runres;
}



/**
* shows an error to stderr if res is nonzero
*/
int lh_showerror_stderr(Lua * lua, int res) {
  if (res<0) {
    fprintf(stderr, "File not found.\n");
    return res;
  }
  return lh_report_stderr(lua, res);
}

/**
* shows an error to console if res is nonzero
*/
int lh_showerror_console(Lua * lua, int res, void * console) {
  if (res<0) {
    fprintf(stderr, "File not found.\n");
    return res;
  }
  return lh_report_console(lua, res, console);
}

/**
* Executes a file in Eruta's data/script directory, and displays any errors
* on stderr. 
*/
int lh_dofile_stderr(Lua * lua, const char * filename) {
  int res;
  // try to load the lua file
  res = lh_dofile(lua, filename);
  lh_showerror_stderr(lua, res);
  return res;
}  

/**
* Executes a file in Eruta's data/script directory, and displays any errors
* on console. 
*/
int lh_dofile_console(Lua * lua, const char * filename, void * console) {
  int res;
  // try to load the lua file
  res = lh_dofile(lua, filename);
  lh_showerror_stderr(lua, res);
  return res;
}  


/**
* Executes a string and displays any errors
* on stderr 
*/
int lh_dostring_stderr(Lua * lua, const char * code) {
  int res;
  // try to load the lua file
  res = lh_dostring(lua, code);
  lh_showerror_stderr(lua, res);
  return res;
}  


/**
* Executes a function and displays any errors
* on console. 
*/
int lh_dofunction_console(Lua * lua, const char * name, void * console) {
  int res;
  // try to load the lua file
  res = lh_dofunction(lua, name);
  lh_showerror_console(lua, res, console);
  return res;
}  


/**
* Executes a string and displays any errors
* on stderr 
*/
int lh_dofunction_stderr(Lua * lua, const char * name) {
  int res;
  // try to load the lua file
  res = lh_dostring(lua, name);
  lh_showerror_stderr(lua, res);
  return res;
}  


/**
* Executes a string and displays any errors
* on console. 
*/
int lh_dostring_console(Lua * lua, const char * code, void * console) {
  int res;
  // try to load the lua file
  res = lh_dostring(lua, code);
  lh_showerror_console(lua, res, console);
  return res;
}  

/** Looks up the console in the lua state's registry and then reports to that,
 as code is executed. */
int lh_dostring_myconsole(Lua * lua, const char * code) {
  Console * console = lh_registry_getptr(lua, "eruta.state.console");
  if (console) return lh_dostring_console(lua, code, console);
  return lh_dostring_stderr(lua, code);
}



/** Looks up the console in the lua state's registry and then reports to that,
 as  the namef function is executed. */
int lh_dofunction_myconsole(Lua * lua, const char * name) {
  Console * console = lh_registry_getptr(lua, "eruta.state.console");
  if (console) return lh_dofunction_console(lua, name, console);
  return lh_dofunction_stderr(lua, name);
}

/*    
void lua_pushinteger (Lua *L, lua_Integer n);
void lua_pushlightuserdata (Lua *L, void *p);
void lua_pushnil (Lua *L);
void lua_pushnumber (Lua *L, lua_Number n);
const char *lua_pushstring (Lua *L, const char *s);
int lua_pushthread (Lua *L);
void lua_pushvalue (Lua *L, int index);
void lua_pushcfunction (Lua *L, lua_CFunction f);
void lua_pushcclosure (Lua *L, lua_CFunction fn, int n);
void lua_pushboolean (Lua *L, int b);
*/



/** Pushes values on top of the lua stack with the given argument list. Uses
* a printf-like interface so it's easier to pass around arguments.
* Returns the amount of values pushed.
* Format can contain: 'd' double, 'i' int, 'b' boolean, 's', string
* 'S' STR (allegro string) , 'p' light user data, 'f': lua_CFunction,
* '0': nil, 'v': lua_pushvalue with integer index. 
*/
int lh_push_va(Lua *L, const char * format, va_list args) {
  const char * c;  
  int index = 0;
  int nargs = 0;
  for (c = format; c && (*c) != '\0' && (index <= nargs); c++, index++) {
    char t = (*c);
    switch(t) {
      case 'd':  {
          double d = va_arg(args, double);
          lua_pushnumber(L, d);
          nargs++;
      }
      break;
      case 'i': {
          int i = va_arg(args, int);
          lua_pushinteger(L, i);
          nargs++;
      }
      case 'b': {
          int i = va_arg(args, int);
          lua_pushboolean(L, i);
          nargs++;
      }
      break;
      case 's': {
        char * s = va_arg(args, char*);
        lua_pushstring(L, s);
        nargs++;
      }
      break;
      case 'S': {
        STR * s = va_arg(args, STR*);
        lua_pushstring(L, str_c(s));
        nargs++;
      }
      break;
      case 'p': {
        void * p;
        p = va_arg(args, void*);
        lua_pushlightuserdata(L, p);
        nargs++;
      }
      case 'f': {
        lua_CFunction f;
        f = va_arg(args, lua_CFunction);
        lua_pushcfunction(L, f);
        nargs++;
      }
      break;
      case '0': lua_pushnil(L); nargs++;
      break;
      case 'v': {
          int i = va_arg(args, int);
          lua_pushvalue(L, i);
          nargs++;
      }
      break;
      default:
      // unknown does nothing.
      break;
    }
  }
  return nargs;
}

/** Pushes values on top of the lua stack with the given arguments. Uses
* printf-like interface so it's easier to pass around arguments.
* Returns the amount of values pushed.
*/
int lh_push_args(Lua *L, const char * format, ...) {
  int result;
  va_list args;
  va_start(args, format);
  result = lh_push_va(L, format, args);
  va_end(args);
  return result;
}  

/** Calls a function  after first calling lh_push_va with the
* argument list. The function should already be on the stack.
* LUA_MULTRET is used as number of results
*/
int lh_call_va(Lua * L, const char * format, va_list args) {
  int nargs = lh_push_va(L, format, args);
  lua_call(L, nargs, LUA_MULTRET);
  return nargs;
}

/** Calls a function  after first calling lh_push_va with the
* arguments. The function should already be on the stack.
*/
int lh_call_args(Lua * L, const char * format, ...) {
  va_list args;
  int nargs;
  va_start(args, format);  
  nargs = lh_call_va(L, format, args);
  va_end(args);  
  return nargs;
}

/** Calls a function  after first calling lh_push_va with the
* argument list. The function should already be on the stack.
* LUA_MULTRET is used as number of results
*/
int lh_pcall_va(Lua * L, const char * format, va_list args) {
  int nargs = lh_push_va(L, format, args);
  return lua_pcall(L, nargs, LUA_MULTRET, 0);
}

/** Calls a function  after first calling lh_push_va with the
* arguments. The function should already be on the stack.
*/
int lh_pcall_args(Lua * L, const char * format, ...) {
  va_list args;
  int res;
  va_start(args, format);
  res = lh_pcall_va(L, format, args);
  va_end(args);
  return res;
}

/** Calls a named global function, as per lh_call_va */
int lh_callglobal_va(Lua * L, const char * name,
                     const char * format, va_list args) {
  lua_getglobal(L, name);                   
  return lh_pcall_va(L, format, args);
}

/** Calls a named global function, as per lh_call_args */
int lh_callglobal_args(Lua * L, const char * name,
                       const char * format, ...) {
  VA_MAP_START(int, format);
  VA_MAP_RESULT(lh_callglobal_va(L, name, format, VA_MAP_ARGS));
  VA_MAP_END();
}

/** Calls a named global function for the lua interpreter,
show errors on stderr */
int lh_callglobalstderr_va(Lua * L, const char * name,
                        const char * format, va_list args) {
  int res = lh_callglobal_va(L, name, format, args);
  lh_showerror_stderr(L, res);
  return res;
}

/** Calls a named global function for the lua interpreter,
show errors on stderr */
int lh_callglobalstderr_args(Lua * L, const char * name,
                        const char * format, ...) {
  VA_MAP_START(int, format);
  VA_MAP_RESULT(lh_callglobal_va(L, name, format, VA_MAP_ARGS));
  VA_MAP_END();
}  
  

/* */     
     
/** Registers the methods and metamethods of the named type. */
int lh_register_data(Lua *L, const char * name,
                     luaL_Reg * methods, luaL_Reg * meta) {
  lua_newtable(L);
  luaL_setfuncs(L, methods, 0);      /* Create methods table,
                                                add it to the globals */
  luaL_newmetatable(L, name);         /* Create metatable for name,
                                         add it to the Lua registry */
  luaL_setfuncs(L, meta, 0);          /* Fill metatable */
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* metatable.__index = methods */
  lua_pushliteral(L, "__metatable");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* hide metatable:
                                         metatable.__metatable = methods */
  lua_pop(L, 1);                      /* drop metatable */
  return 1;                           /* return methods on the stack */
}

// need luaL_requiref(L, lib->name, lib->func, 1); somewhere

/** Like lua_rawseti, but with a string key. */
void lh_rawsets(Lua *L, char * key, int index) {
  lua_pushstring(L, key);
  lua_insert(L, -2); // swap top 2 entries on stack (value is there) 
  lua_rawset(L, index); // Set into table.
}

/** Like lua_rawgeti but with a string key */
void lh_rawgets(Lua *L, int index, char * key) {
  lua_pushstring(L, key); 
  lua_rawget(L, index); // Get from table.
}

/**Calls luaL_checktype() with a LUA_TTABLE argument. */
void lh_checktable(Lua *L, int index) {
  luaL_checktype(L, index, LUA_TTABLE);
}

/** Utility function that calls lua_rawgeti, and then lua_tointeger and lua_pop and returns the result. In other words, it gets an integer 
value at an integer index key from the table at index on the lua stack.
**/
int lh_geti(Lua *L, int index, int key) {
  int result, ok;
  lua_rawgeti(L, index, key);
  // Get integer from top of stack
  result = lua_tointegerx(L, -1, &ok);
  lua_pop(L, 1);
  if(!ok) return -1;
  return result;
}


/** Makes a new metatable for the given type name, and prepares it so the data will be able to given methods using the lh_datamethod function. If fun is not NULL, it sets the __gc value in the metatable to fun, so data of the given type will be correctly garbage collected. Must be called before calling
lh_datamethod.
*/
void lh_datainit(Lua *L, char * name, lua_CFunction fun) {
  luaL_newmetatable(L, name);
  // new table for methods, in __index
  lua_newtable(L);
  lh_rawsets(L, "__index", -3);
  if(!fun) return;
  lua_pushcfunction(L, fun);
  lh_rawsets(L, "__gc", -3);
}

/** adds a method to the given named user data type. */
void lh_datamethod(Lua *L, char * meta, char * name, lua_CFunction fun) {
  luaL_getmetatable(L, meta);
  lh_rawgets(L,   -2, "__index");
  // Get the method-lookup table in the metatable.
  lua_pushcfunction(L, fun);
  lh_rawsets(L, name , -3);
}


/** Gets a string that defines the name of the type at stack index index. */
const char * lh_typename(Lua *L, int index) {
  return lua_typename(L, lua_type(L, index));
}

/** Walks over a table at index, calling the walker function as it goes.
* key will be at index -2, and value at index -1 
*/
void lh_tablewalk(Lua *L, int index,
                  lh_walker * walker, void * data) {
  /* table is in the stack at index 'index' */
  lua_pushnil(L);  /* first key */
  while (lua_next(L, index) != 0) {
    /* uses 'key' (at index -2) and 'value' (at index -1) */
    walker(L, data);
    /* removes 'value'; keeps 'key' for next iteration */
    lua_pop(L, 1);
  }  
}

/** Adds a global integer constant to the lua state */
void lh_globalint(Lua *L, const char *name, const int i) {
  lua_pushinteger(L, i);
  lua_setglobal(L, name);
}




/** Stores a C pointer in the registry with the given string key */
void lh_registry_putptr(Lua *L, const char * name, void * ptr) {
    lua_pushstring(L, name);       /* push address */
    lua_pushlightuserdata(L, ptr); /* push value   */
    lua_settable(L, LUA_REGISTRYINDEX);
}

/** Retrieves a c pointer from the registry with the given string key */
void * lh_registry_getptr(Lua *L, const char * name) { 
    lua_pushstring(L, name);              /* push address */
    lua_gettable(L, LUA_REGISTRYINDEX);   /* retrieve value */
    return lua_touserdata(L, -1);    /* get light user data */
}



