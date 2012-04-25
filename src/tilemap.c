#include <chipmunk.h>
// #include <mxml.h>
#include "mem.h"
#include "image.h"
#include "tilemap.h"
#include "dynar.h"

#define TILEMAP_PANES 4

/** A tile map is a game map that uses tiled panes for it's 
display and physics. */
struct Tilemap_ {
  Image     * texture;
  int         textureid;
  int         gridw;
  int         gridh;
  Tileset   * set;
  Dynar     * panes;
  cpSpace   * space;
};

#define TEXTURE_TILE    "tile"
#define TEXTURE_PLAYER  "play"
#define TEXTURE_GEAR    "gear"
#define TEXTURE_WEAPON  "arms"
#define TEXTURE_FOE     "foes"
#define TEXTURE_ITEM    "item"

/** Loads an image of the given category and index as a texture. */
Image * image_loadtexture(const char * category, int index) {
  char buf[1024];
  Image   * image;
  sprintf(buf, "data/image/%s_%03d.png", category, index);
  image   = image_load(buf);
  return image;
}

/** Cleans up a tilemap. */
Tilemap * tilemap_done(Tilemap * self) {
  int index;
  if(!self) return NULL;
  for(index = 0; index < TILEMAP_PANES; index++) {
    tilepane_free(dynar_getptr(self->panes, index));
  }
  dynar_free(self->panes);
  cpSpaceFree(self->space);
  tileset_free(self->set);
  // tilemap_initempty(self);
  return self;
}


/** Initializes a tile map */
Tilemap* tilemap_init(Tilemap * self, Tileset * set, int w, int h) {
  int index;
  if(!self) return NULL;
  self->gridw     = w;
  self->gridh     = h;
  self->set       = set;
  if(!self->set)   { return NULL; }
  self->panes     = dynar_new(TILEMAP_PANES, sizeof(Tilepane *));
  if(!self->panes) { return NULL; }   
  for(index = 0; index < TILEMAP_PANES; index++) {
    dynar_putptr(self->panes, index, NULL);
  }
  self->space = cpSpaceNew();
  return self;
}

/** Frees the tile map and initializes it. */
Tilemap * tilemap_free(Tilemap * map) {
  tilemap_done(map);
  mem_free(map);
  return NULL;
}

/** Allocates a new tile map and initializes it. */
Tilemap * tilemap_new(Tileset * set, int w, int h) {
  Tilemap * map = STRUCT_ALLOC(Tilemap);
  if(!tilemap_init(map, set, w, h)) {
    return tilemap_free(map);
  }
  return map;
}


/** Returns a pointer to the pane at index or NULL if out of range. */
Tilepane * tilemap_pane(Tilemap * self, int index) {
  return dynar_getptr(self->panes, index);
}

/** Sets a new tile pane for the pane at indexeth index of the tile map.
* the old pane, if any, will be deleted with tilepane_free
*/
Tilepane * tilemap_pane_(Tilemap * self, int index, Tilepane * pane) {
  Tilepane * oldpane;
  oldpane = tilemap_pane(self, index);
  // Replace old pane, so free it.
  tilepane_free(oldpane);  
  dynar_putptr(self->panes, index, pane);
  return pane;
}

/** Makes a new tile pane for the pane at indexeth index of the tile map. */
Tilepane * tilemap_panenew(Tilemap * self, int index, int w, int h) {
  Tilepane * pane = tilepane_new(self->set, w, h);
  return tilemap_pane_(self, index, pane);
}


/** Returns the tile in the tile map in the given layer at the given coords. */
Tile * tilemap_get(Tilemap * self, int l, int x, int y) {
  Tilepane * pane = tilemap_pane(self, l);
  if(!pane) return NULL;
  return tilepane_get(pane, x, y);
}

/** Sets a tile in the tile map to the given tile. */
Tile * tilemap_settile(Tilemap * self, int l, int x, int y, Tile * tile) {
  Tilepane * pane = tilemap_pane(self, l);
  if(!pane) return NULL;
  if(!tilepane_set(pane, x, y, tile)) return NULL;
  // TODO: Add physical wall if it's a wall.
  /*
  if(tilemap_tilewall(self, tile)) {
    tilemap_makewall(self, x, y);
  }
  */
  return tile;
}

/** Sets a tile in the tile map to the tile with the given index. */
Tile * tilemap_setindex(Tilemap * self, int l, int x, int y, int index) {
  Tilepane * pane = tilemap_pane(self, l);
  Tile     * tile = NULL;
  if(!pane) return NULL;
  tile            = tilepane_setindex(pane, x, y, index);
  if(!tile) return NULL;
  // TODO: Add physical wall if it's a wall.
  /*
  if(tilemap_tilewall(self, tile)) {
    tilemap_makewall(self, x, y);
  }
  */
  return tile;
}

/** Sets a rectangle area in the tile map to the given tile. */
Tile * tilemap_rect(Tilemap * self, int l, 
                       int x, int y, int w, int h, Tile * tile) {
  Tilepane * pane = tilemap_pane(self, l);
  if(!pane) return NULL;
  return tilepane_rect(pane, x, y, w, h, tile);
}

/** Sets a rectangle area in the tile map to the given tile. */
Tile * tilemap_fill(Tilemap * self, int l, Tile * tile) {
  Tilepane * pane = tilemap_pane(self, l);
  return tilepane_fill(pane, tile);
}

/** Gets the index of the tile at the given location in the tilemap. */
int tilemap_getindex(Tilemap * self, int l, int x, int y) {
  Tilepane * pane = tilemap_pane(self, l);
  return (int) tilepane_getindex(pane, x, y);
}

/** Makees a physical wall in the chipmunk space of the map at the given 
tile coordinates. Returns the wall shape. */
cpShape * tilemap_makewall(Tilemap * self, int tx, int ty) {
  cpFloat x     = (cpFloat)(tx * TILE_W);
  cpFloat y     = (cpFloat)(ty * TILE_H);
  cpFloat w     = (cpFloat)(TILE_W);
  cpFloat h     = (cpFloat)(TILE_H);
  cpBody *body  = cpSpaceGetStaticBody(self->space);
  cpVect  p[4]  = { 
                    cpv(x   , y  ), 
                    cpv(x   , y+h), 
                    cpv(x+w , y+h), 
                    cpv(x+w , y)
                   }; 
  cpShape * wall= cpPolyShapeNew(body, 4, p, cpvzero);
  cpSpaceAddShape(self->space, wall);
  return wall;
}

/** Draws a tile map. */
void tilemap_draw(Tilemap * map, Camera * camera) {
  int index;
  Tilepane * pane;
  for(index  = 0; index < TILEMAP_PANES; index++) {
    pane     = tilemap_pane(map, index);
    if(pane) {
      tilepane_draw(pane, camera);
    } else {
      // fprintf(stderr, "pane missing: %d", index);
    }
  }
}




#ifdef COMMENT_
Tilemap * tilemap_load(lua_State * lua, int index) {  
  char buf[1024];
  sprintf(buf, "data/script/map_%04d.lua", index);
  if(luaL_dofile(lua, buf)) {
    printf("Error: %s\n", lua_tostring(lua, 1)); 
    return NULL;
  }  
  sprintf(buf, "data/script/mex_%04d.lua", index);
  if(!luaL_dofile(lua, buf)) {
    printf("Warning: %s\n", lua_tostring(lua, 1));
    // xxx: should display some error here...
  }
  return tilemap_getcurrent();
}


Tilepane * tilepane_savefile(Tilepane *pane, int paneid, FILE *fout) {
  int xx, yy, gw, gh;
  gw = tilepane_gridwide(pane);
  gh = tilepane_gridhigh(pane);
  for(yy = 0; yy < gh; yy++) {
    fprintf(fout, "mapsetrow{ %d, %d ", paneid, yy);    
    for(xx = 0; xx < gw; xx++) {
      int tile = (int) tilepane_get(pane, xx, yy);
      fprintf(fout, ", %d", tile);
    }
    fprintf(fout, "}\n");
  }
  return pane;
}

Tilemap * tilemap_savefile(Tilemap * map, FILE * fout) {
  int index, tds;
  if(!fout) return NULL;
  fprintf(fout, "-- Automatical save of map, DO NOT HAND EDIT.\n");
  fprintf(fout, "-- In stead, edit mex_xxxx.lua .\n--\n");  
  fprintf(fout, "-- Initialize map.\n");
  fprintf(fout, "map = mapinit(%d, %d, %d)\n", 
          map->textureid, map->gridw, map->gridh);
  /* save tile info */
  fprintf(fout, "-- Tile info.\n");
  /*
  tds = gytiledata_size(map->tiledata);
  fprintf(fout, "maptiledata { ");
  for (index = 0; index < tds; index++) {
    int info = tilemap_tileinfo(map, index);
    if(info) { 
      fprintf(fout, "[%d] = %d; ", index, info);
      //fprintf(fout, "maptileinfo(%d, %d)\n", index, info);
    } 
  }
  */
  fprintf(fout, "}\n");
  fprintf(fout, "-- Layer data.\n");        
  for(index = 0; index < TILEMAP_PANES; index++) {
    fprintf(fout, "-- Layer nr %d.\n", index);
    fprintf(fout, "mapfill(%d, %d)\n", index, -1);
    tilepane_savefile(tilemap_pane(map, index), index, fout);
  }
  return map;
}


Tilemap * tilemap_save(Tilemap * map, int index) {
  char buf[1024];
  FILE * fout;
  Tilemap * res;
  sprintf(buf, "data/script/map_%04d.lua", index);
  fout = fopen(buf, "wt");
  res  = tilemap_savefile(map, fout);
  fclose(fout);
  return res;
  
}


#endif
