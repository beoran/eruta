/* Lua functionality for basewars. */
#include "lh.h" 
#include "basewars.h"

/** Al functions have a lb prefix that stands for "lua basewars" */

/** Initializes the map. */
int lb_mapinit(lua_State * lua) {
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
int lb_mapset(lua_State * lua) {
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
int lb_mapsetrow(lua_State * lua) {
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
int lb_mapfill(lua_State * lua) {
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
int lb_maptileinfo(lua_State * lua) {
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
int lb_maptiledata_walker(lua_State * lua, void * data) {
  int tile, info;
  BaMap * map = (BaMap *) data;
  tile        = lua_tointeger(lua, -2);
  info        = lua_tointeger(lua, -1);
  bamap_tileinfo_(map, tile, info);
  return 0;
}

/** Sets multiple tile infos from a hash to a map. */
int lb_maptiledata(lua_State * lua) {
  BaMap * map = bamap_getcurrent();
  if(!map) {
    return luaL_error(lua, "NULL pointer to current map!.");
  }
  lh_checktable(lua, 1);
  lh_tablewalk(lua, 1, lb_maptiledata_walker, map);
  return 0;
}



int lb_init(lua_State * lua) {
  luaL_openlibs(lua);
  // luaL_dostring(lua, "print 'Hello!' "); 
  lua_register(lua, "mapinit"     , lb_mapinit);
  lua_register(lua, "mapset"      , lb_mapset);
  lua_register(lua, "mapsetrow"   , lb_mapsetrow);
  lua_register(lua, "mapfill"     , lb_mapfill);
  lua_register(lua, "maptileinfo" , lb_maptileinfo);
  lua_register(lua, "maptiledata" , lb_maptiledata);
  lh_globalint(lua, "TILE_WALL"   , 1);
}






