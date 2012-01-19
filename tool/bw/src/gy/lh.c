/** Lua helper functions. */
#include <stdlib.h>
#include <string.h>
#include <lauxlib.h>
#include "lh.h"

/* Not defined in lauxlib somehow... */
void luaL_openlibs (lua_State *L);

/** Lua helper functions and macros. */
lua_State * lh_new() {
  lua_State * lua = luaL_newstate();
  if(!lua) {
    return NULL;
  }
  luaL_openlibs(lua);
  return lua;
}

lua_State * lh_free(lua_State * lua) {
  if(lua) lua_close(lua);
  return NULL;
}


/** Creates lua user data in which the pointer to data will be stored. */
void ** lh_pushdata(lua_State *L, const char * name, void * data) { 
  void ** ptr = lua_newuserdata(L, sizeof(void*));
  (*ptr)      = data;
  luaL_getmetatable(L, name);
  lua_setmetatable(L, -2);
  return ptr;
}

/** Gets the object at stack index index as a pointer to data */
void * lh_todata(lua_State *L,  int index) {
  void **ptr = (void **)lua_touserdata(L, index);
  if (!ptr) { 
    fprintf(stderr, "Argument %i is NULL.\n", index);
    luaL_error(L, "Argument %i is NULL.\n", index);
    return NULL;
  }  
  return *ptr;
}

/** Gets the object at stack index index as a pointer to data. 
Performs type checking. */
void * lh_checkdata(lua_State *L, const char * name, int index) {
  void *data;
  luaL_checkudata (L, index, name);
  // 
  data = lh_todata(L, index);
  if (!data) { 
   fprintf(stderr, "Data NULL is NULL.");
    luaL_error(L, "Data may not be NULL");
    return NULL;
  }  
  return data;
}

int lh_scanargs(lua_State *L, char * format, ...) {
  va_list args; 
  char * c;
  int index = 1;
  int nargs = lua_gettop(L); 
  va_start(args, format);
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
      case 'S': {
        char ** s;
        s = va_arg(args, char**);
        if (s) { (*s) = strdup(luaL_checkstring(L, index)); }
      }
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
  va_end(args);
  return 0;
};


/** Registers the methods and metamethods of the named type. */
int lh_register_data(lua_State *L, const char * name, 
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
void lh_rawsets(lua_State *L, char * key, int index) {
  lua_pushstring(L, key);
  lua_insert(L, -2); // swap top 2 entries on stack (value is there) 
  lua_rawset(L, index); // Set into table.
}

/** Like lua_rawgeti but with a string key */
void lh_rawgets(lua_State *L, int index, char * key) {
  lua_pushstring(L, key); 
  lua_rawget(L, index); // Get from table.
}

/**Calls luaL_checktype() with a LUA_TTABLE argument. */
void lh_checktable(lua_State *L, int index) {
  luaL_checktype(L, index, LUA_TTABLE);
}

/** Utility function that calls lua_rawgeti, and then lua_tointeger and lua_pop and returns the result. In other words, it gets an integer 
value at an integer index key from the table at index on the lua stack.
**/
int lh_geti(lua_State *L, int index, int key) {
  int result, ok;
  lua_rawgeti(L, index, key);
  // Get integer from top of stack
  result = lua_tointegerx(L, -1, &ok);
  lua_pop(L, 1);
  if(!ok) return -1;
  return result;
}


/** Makes a new metatable for the given type name, and prepares it so the data will be able to given methods usin gthe lh_datamethod function. If fun isn ot NULL, it sets the __gc value in the metatable to fun, so data of the given type will be correctly garbage collected. Must be called before calling 
lh_datamethod.
*/
void lh_datainit(lua_State *L, char * name, lua_CFunction fun) {
  luaL_newmetatable(L, name);
  // new table for methods, in __index
  lua_newtable(L);
  lh_rawsets(L, "__index", -3);
  if(!fun) return;
  lua_pushcfunction(L, fun);
  lh_rawsets(L, "__gc", -3);
}

/** adds a method to the given named user data type. */
void lh_datamethod(lua_State *L, char * meta, char * name, lua_CFunction fun) {
  luaL_getmetatable(L, meta);
  lh_rawgets(L,   -2, "__index");
  // Get the method-lookup table in the metatable.
  lua_pushcfunction(L, fun);
  lh_rawsets(L, name , -3);
}


/** Gets a string that defines the name of the type at stack index index. */
const char * lh_typename(lua_State *L, int index) { 
  return lua_typename(L, lua_type(L, index));
}

/** Walks over a table at index, calling the walker function as it goes.
* key will be at index -2, and value at index -1 
*/
void lh_tablewalk(lua_State *L, int index, 
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
void lh_globalint(lua_State *L, const char *name, const int i) {
  lua_pushinteger(L, i);
  lua_setglobal(L, name);
}


