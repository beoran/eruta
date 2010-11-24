
#include "gari.h"

#include "lauxlib.h"
#include "lualib.h"
#include "lua.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"
  

/** Lua interface for Gari (and vice versa). 
* Since Ruby is hard to embed and Lua is fater, and I've used it before,
* and it's kind of famous for use in games, I'd like to try it out and 
* see if it's not easier than ruby in the end. 
*
*/

// wrap the pointer a bit
typedef lua_State * GariLua;

// some other typedefs for wrapping  
typedef luaL_Reg GariLuaReg;
typedef lua_CFunction gari_lua_function;


#define GARI_LUA_STATE(STATE)  (lua_State *)(STATE)
#define GARI_LUA_GARI(STATE)  (GariLua)(STATE)

#define lua_boxpointer(L, U) \
        (*(void **)(lua_newuserdata(L, sizeof(void *))) = (U))

#define lua_unboxpointer(L, I)   (*(void **)(lua_touserdata(L, U)))


void gari_lua_free(GariLua lua) {
  if(lua) { lua_close(lua); }
}

GariLua gari_lua_new() {
    return GARI_LUA_GARI(luaL_newstate()); 
}

// plenty of macros and functions for wrapping

// wraps a GARI function's header  
#define GARI_LUA_WRAPNAME(FUNC) FUNC##_lua

#define GARI_LUA_FUNC(FUNC) GARI_FUNC(int)\
        GARI_LUA_WRAPNAME(FUNC) (lua_State * lua) 
        
void * gari_lua_getstruct(GariLua lua, int index, const char * name) {
  return (void*)(luaL_checkudata(GARI_LUA_STATE(lua), index, name));
}

#define GARI_LUA_CHECKUDATA(LUA, NAME, INDEX)\
        luaL_checkudata(LUA, INDEX, NAME)
        
#define GARI_LUA_STRUCTNAME(STRUCT) #STRUCT
 
#define GARI_LUA_GETUDATA(LUA, STRUCT, INDEX) \
        (STRUCT **)(GARI_LUA_CHECKUDATA(LUA, GARI_LUA_STRUCTNAME(STRUCT), INDEX))

// Allocates a new void** pointer that can be usd to store userdata in.
// the void** pointer points to an address that is pointing in turn to NULL. 
void ** gari_lua_newuserdata(GariLua lua, const char * name) {
  void ** result;
  result         = (void**)lua_newuserdata(lua, sizeof(void *));
  if(!result)    return NULL;
  *result        = NULL;
  luaL_getmetatable(lua, name);
  lua_setmetatable(lua, -2);
  return result; 
}

// Allocates a void** pointer and wraps ptr in it.
void ** gari_lua_wrapuserdata(GariLua lua, const char * name, void * ptr) {
  void ** result; 
  result = gari_lua_newuserdata(lua, name);
  if(!result) return NULL;
  *result = ptr;
  return result;
} 

#define GARI_LUA_WRAP(LUA, STRUCT, PTR)\
        gari_lua_wrapuserdata(LUA, GARI_LUA_STRUCTNAME(STRUCT), PTR)

// Creates a metatable for a given type name, and fills in the methods
// from the reg table, so the type name will have the needed methods.
// especially __gc is important! 
void gari_lua_makemethods(GariLua lua, const char * name, GariLuaReg * reg) {
  luaL_register(lua, name,  reg);     /* methods for the type */
}

/** 
* Initializes Gari Game and returns it. Must be done before anything else. 
*/
GariGame * gari_game_make();

/** Finishes and cleans up a gari game when we're done with it. */
void gari_game_free(GariGame * game);

/** Updates the game screen, FPS, etc. */
GariGame * gari_game_update(GariGame * game); 

/** Sets the frames of the game back to 0.*/
GariGame * gari_game_resetframes(GariGame * game); 

/** Gets the total amount of frames displayed since the start of the game, 
   or since gari_game_resetframes was called. */
uint32_t gari_game_frames(GariGame * game);

/** Advances the game's frame counter. */
GariGame * gari_game_nextframe(GariGame * game);

/** Starts FPS counter. gari_game_resetframes also calls this. */
GariGame * gari_game_startfps(GariGame * game);

/** Returns calculated fps after calling startfps . */
double gari_game_fps(GariGame * game);

/** Reports on the game's FPS on stderr.*/
void gari_game_report(GariGame * game);

/** Retuns the current main game screen. */
GariScreen * gari_game_screen(GariGame * game);

/** Returns the drawable image for the screen. */
GariImage * gari_screen_image(GariScreen * screen);

/** Opens the game main game screen or window. 
   Must be done before using any image functions. */   
GariScreen * screen_init_depth(GariGame * game, int wide, int high, int fullscreen, int depth);   

 
/* 
GariLua * gari_lua_wrap(GariLua * lua, void * ptr, 
                        char * name  , GariLuaFree * free) {
  luaL_newmetatable(lua->state, name);
  
}
 

GARI_LUA_FUNC(gari_game_make) {
  GariGame * game = gari_game_make();
  GARI_LUA_WRAP(lua, GariGame, game);
  return 1;
}

GARI_LUA_FUNC(gari_game_free) {
  GariGame ** game = GARI_LUA_GETUDATA(lua, -1, GariGame);  
  gari_game_free(*game);
  return 1;
}


GARI_LUA_FUNC(gari_screen_image) {
  return 0;
}

GariLuaReg game_methods[] = { { "__gc", gari_game_free_lua} 
                              ,  { NULL, NULL} };

GariLua gari_lua_setup(GariLua lua) {
  gari_lua_makemethods(lua, "GariGame", game_methods);  
  return lua;
} 

*/









