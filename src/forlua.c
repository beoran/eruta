/* Functions that extend lua for use with Eruta. */
#include "lh.h" 
#include "eruta.h"
#include "fifi.h"


/* All functions have a fl prefix that stands for "Function for Lua" */

#ifdef COMMENT_

/** Initializes the map. */
int fl_mapinit(lua_State * lua) {
  int texid = -1, w = -1, h = -1;
  BaMap * map = bamap_getcurrent();
  if(!map) {
    printf("NULL pointer to current map!.\n");
    return luaL_error(lua, "NULL pointer to current map!.");
  }
  
  lh_scanargs(lua, "iii", &texid, &w, &h);
  if((texid < 1) || (w < 1) || (h <1)) {
    printf("Mapinit arguments not valid.\n");
    return luaL_error(lua, "mapinit() arguments not valid.");
  }
  bamap_done(map);                // deinitialize map;
  bamap_init(map, texid, w , h); // initialize map with new parameters;
  return 0;
}

/** Sets a single tile in the map. */
int fl_mapset(lua_State * lua) {
  int l = -1, x = -1, y = -1, tile = -1;
  BaMap * map = bamap_getcurrent();
  if(!map) {
    return luaL_error(lua, "NULL pointer to current map!.");
  }

  lh_scanargs(lua, "iiii", &l, &x, &y, &tile);
  if ((l < 0) || (x < 0) || (y < 0)) {
    return luaL_error(lua, "mapset() arguments not valid.");
  }
  bamap_set(map, l, x, y, tile);
  return 0;
}

/** Sets a row of tiles in the map. */
int fl_mapsetrow(lua_State * lua) {
  int x, y, layer, index, stop, tile; 
  BaMap * map = bamap_getcurrent();
  if(!map) {
    return luaL_error(lua, "NULL pointer to current map!.");
  }
  lh_checktable(lua, 1);
  stop = luaL_len(lua, 1);
  layer= lh_geti(lua, 1, 1);
  y    = lh_geti(lua, 1, 2);
  for(index = 3; index <= stop; index ++) {
    tile = lh_geti(lua, 1, index);
    x    = index - 3;
    if ((layer < 0) || (x < 0) || (y < 0)) {
      return luaL_error(lua, "mapsetrow() arguments not valid: %d, %d, %d.", layer, x, y);
    }
    bamap_set(map, layer, x, y, tile);  
  }
  return 0;
}

/** Fills a whole layer in the map. */
int fl_mapfill(lua_State * lua) {
  int l = -1, tile = -1;
  BaMap * map      = bamap_getcurrent();
  if(!map) {
    return luaL_error(lua, "NULL pointer to current map!.");
  }
  lh_scanargs(lua, "ii", &l, &tile);
  if ((l < 0)) {
    return luaL_error(lua, "mapfill() arguments not valid.");
  }
  bamap_fill(map, l, tile);
  return 0;
}

/** Sets tile info for the map. */
int fl_maptileinfo(lua_State * lua) {
  int tile = -1, info = -1;
  BaMap * map = bamap_getcurrent();
  if(!map) {
    return luaL_error(lua, "NULL pointer to current map!.");
  }

  lh_scanargs(lua, "ii", &tile, &info);
  if ((tile < 0) || (info < 0)) {
    return luaL_error(lua, "maptileinfo() arguments not valid.");
  }
  bamap_tileinfo_(map, tile, info);
  return 0;
}  

/** Walker that sets multiple tile infos from a hash to a map. */
int fl_maptiledata_walker(lua_State * lua, void * data) {
  int tile, info;
  BaMap * map = (BaMap *) data;
  tile        = lua_tointeger(lua, -2);
  info        = lua_tointeger(lua, -1);
  bamap_tileinfo_(map, tile, info);
  return 0;
}

/** Sets multiple tile infos from a hash to a map. */
int fl_maptiledata(lua_State * lua) {
  BaMap * map = bamap_getcurrent();
  if(!map) {
    return luaL_error(lua, "NULL pointer to current map!.");
  }
  lh_checktable(lua, 1);
  lh_tablewalk(lua, 1, fl_maptiledata_walker, map);
  return 0;
}




#endif

int fl_test(lua_State * lua) {
  printf("Hello from test!\n");
  return 0;
}

int fl_Path_gc(Lua * lua) {
  ALLEGRO_PATH * path = NULL;
  lh_scanargs(lua, "p", &path);
  printf("fl_Path_gc : %s\n", PATH_CSTR(path));
  al_destroy_path(path);
  return 0;
}

int fl_Path_to_s(Lua * lua) {
  ALLEGRO_PATH * path = NULL;
  lh_scanargs(lua, "p", &path);  
  printf("fl_Path_to_s : %s\n", PATH_CSTR(path));
  if(!path) { lua_pushnil(lua); }
  else { lua_pushstring(lua, PATH_CSTR(path)); }
  return 1;
}

void fl_Path_initclass(Lua *lua) { 
  lh_datainit(lua, "Path", fl_Path_gc);
  lh_datamethod(lua, "Path", "to_s"  , fl_Path_to_s);
}

int fl_Path_data(Lua * lua) {
  ALLEGRO_PATH * path = NULL;
  const char * vpath = NULL;
  lh_scanargs(lua, "s", &vpath);
  path = fifi_data_vpath(vpath);
  printf("fl_Path_data : %s (for %s)\n", PATH_CSTR(path), vpath);
  lh_pushdata(lua, "Path", path);
  return 1;
}


/** Initializes the functionality that Eruta exposes to lua. */
int fl_init(lua_State * lua) {
  // luaL_dostring(lua, "print 'Hello!' ");
  lua_register(lua, "test"        , fl_test);
  fl_Path_initclass(lua);
  lua_register(lua, "PathForData" , fl_Path_data);
  lh_globalint(lua, "TILE_WALL"   , 1);
  return 0;
}




