/** Lua helper library */
#ifndef LH_H
#define LH_H

#include <lua.h>
#include <lauxlib.h>


/** For when I get lazy... */
typedef lua_State Lua;

#include "state.h"

/** Opens a lua state with libraries. */
lua_State * lh_new();

/** Closes a lua state with libraries. */
lua_State * lh_free(lua_State * lua);


/** Creates lua user data in which the pointer to data will be stored. */
void ** lh_pushdata(lua_State *L, const char * name, void * data);

/** Gets the object at stack index index as a pointer to data */
void * lh_todata(lua_State *L, int index);

/** Gets the object at stack index index as a pointer to data. 
Performs type checking. */
void * lh_checkdata(lua_State *L, const char * name, int index);

/** Scans in several arguments using the format string. */
int lh_scanargs(lua_State *L, char * format, ...);

#define LH_PUSHDATA(L, TYPE, DATA)    ((TYPE*)lh_pushdata(L,#TYPE,DATA))
#define LH_CHECKDATA(L, TYPE, INDEX)  ((TYPE*)lh_checkdata(L,INDEX,#TYPE))
#define LH_GETDATA(L, TYPE, INDEX)    ((TYPE*)lh_getdata(L,INDEX))
 
/** Registers the methods and metamethods of the named type. */
int lh_register_data(lua_State *L, const char * name, 
                     luaL_Reg * methods, luaL_Reg * meta);

/** Like lua_rawseti, but with a string key. */
void lh_rawsets(lua_State *L, char * key, int index);

/** Like lua_rawgeti but with a string key */
void lh_rawgets(lua_State *L, int index, char * key);

/**Calls luaL_checktype() with a LUA_TTABLE argument. */
void lh_checktable(lua_State *L, int index);

/** Utility function that calls lua_rawgeti, and then lua_tointeger and lua_pop and returns the result. In other words, it gets an integer 
value at an integer index key from the table at index on the lua stack.
**/
int lh_geti(lua_State *L, int index, int key);

/** Makes a new metatable for the given type name, and prepares it so the data will be able to given methods usin gthe lh_datamethod function. If fun isn ot NULL, it sets the __gc value in the metatable to fun, so data of the given type will be correctly garbage collected. Must be called before calling 
lh_datamethod.
*/
void lh_datainit(lua_State *L, char * name, lua_CFunction fun);

/** adds a method to the given named user data type. */
void lh_datamethod(lua_State *L, char * meta, char * name, lua_CFunction fun);

/** gets the type name of the object at the given stack index. */
const char * lh_typename(lua_State *L, int index);

typedef int (lh_walker)(lua_State *L, void * data); 

/** Walks over a table at index, calling the walker function as it goes.
* key will be at index -2, and value at index -1 
*/
void lh_tablewalk(lua_State *L, int index, lh_walker * walker, void * data);

/** Adds a global integer constant to the lua state */
void lh_globalint(lua_State *L, const char *name, const int i);


#endif
