#include <chipmunk.h>
// #include <mxml.h>
#include "mem.h"
#include "image.h"
#include "tilemap.h"
#include "dynar.h"

#define TILEMAP_PANES 4

/* Hide tiles for debuggging, or not. */
#ifdef ERUTA_NOGFX_MODE
#define TILEMAP_NO_TILES
#endif

/** A tile map is a game map, roughly equivalent to a "level" that uses tiled 
 * panes for it's display. It also contains an Area for the physical, logical
 * and graphical representation of in-game objects.  
 * The plan is to have an area per tile map and copy the player characters between 
 * those areas when moved, but keeping their ID's the same.  
 * This should make keeping track of the characters (from scripting, etc) easier.
 */
struct Tilemap_ {
  Image     * texture;
  int         textureid;
  int         gridw;
  int         gridh;
  Tileset   * set;
  Dynar     * panes;
  Area      * area;
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
  tileset_free(self->set);
  /* the area is not owned so we don't free that */
  // tilemap_initempty(self);
  return self;
}


/** Initializes a tile map */
Tilemap* tilemap_init(Tilemap * self, Tileset * set, int w, int h, Area * area) {
  int index;
  if(!self) return NULL;
  self->gridw     = w;
  self->gridh     = h;
  self->set       = set;
  self->area      = area;
  area_tilemap_(area, self);
  if(!self->set)   { return NULL; }
  self->panes     = dynar_new(TILEMAP_PANES, sizeof(Tilepane *));
  if(!self->panes) { return NULL; }   
  for(index = 0; index < TILEMAP_PANES; index++) {
    dynar_putptr(self->panes, index, NULL);
  }
  return self;
}

/** Frees the tile map and initializes it. */
Tilemap * tilemap_free(Tilemap * map) {
  tilemap_done(map);
  mem_free(map);
  return NULL;
}

/** Allocates a new tile map and initializes it. */
Tilemap * tilemap_new(Tileset * set, int w, int h, Area * area ) {
  Tilemap * map = STRUCT_ALLOC(Tilemap);
  if(!tilemap_init(map, set, w, h, area)) {
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


/** Converts the tile's flags to an integer id for the best Thing to 
represent this tile. Returns THING_UNUSED if it needs no physical representation. */
int tile_thingkind(Tile * tile) {
  if (!tile) return THING_UNUSED;
  if (tile_index(tile)<1) return THING_UNUSED;
  if (tile_isflag(tile, TILE_WALL))  return THING_WALL;
  if (tile_isflag(tile, TILE_STAIR)) return THING_STAIR;
  if (tile_isflag(tile, TILE_WATER)) return THING_WATER;
  return THING_UNUSED;  
}

/** Makes sure that the tile gets properly physically represented in the 
Area of the tilemap. Returns the Thing generated or NULL when no physical
representation is needed. With the Bump branch, this isn't needed anymore, 
so always returns NULL.
*/
Thing * tilemap_tiletothing(Tilemap * self, int l, int x, int y, Tile * tile) {
  return NULL;
}


/** Sets a tile in the tile map to the given tile. */
Tile * tilemap_settile(Tilemap * self, int l, int x, int y, Tile * tile) {
  Tile     * aidt = NULL;
  Tilepane * pane = tilemap_pane(self, l);
  if(!pane) return NULL;
  aidt            = tilepane_set(pane, x, y, tile);
  if(!aidt) return NULL;
  return tile;
}

/** Sets a tile in the tile map to the tile with the given index. */
Tile * tilemap_setindex(Tilemap * self, int l, int x, int y, int index) {
  Tilepane * pane = tilemap_pane(self, l);
  Tile     * tile = NULL;
  if(!pane) return NULL;
  tile            = tilepane_setindex(pane, x, y, index);
  if(!tile) return NULL;
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

/** Draws a tile map. */
void tilemap_draw(Tilemap * map, Camera * camera) {
  int index;
  Tilepane * pane;
#ifndef TILEMAP_NO_TILES
  for(index  = 0; index < TILEMAP_PANES; index++) {
    pane     = tilemap_pane(map, index);
    if(pane) {
      tilepane_draw(pane, camera);
    } else {
      // fprintf(stderr, "pane missing: %d", index);
    }
  }
#else 
  al_clear_to_color(al_map_rgb(0,0,0));
#endif
}

/** Updates the tile map. Currently this animates the tiles. */
void tilemap_update(Tilemap * map, double dt) {
  tileset_update(map->set, dt);
}

/** Adds a dynamic thing of the given type to the tile map's area */
Thing * tilemap_addthing(Tilemap * self, int kind, int x, int y, int z, 
                         int w, int h) {
  return area_newdynamic(self->area, kind, x, y, z, w, h);
}


/** Sets up the camera so it will stay locked in to the 
given layer of the tile map */
Lockin * tilepane_lockin(Tilepane * pane, Camera * camera) {
  float x, y, w, h;
  if(!pane) return NULL;
  x = 0.0;
  y = 0.0;
  w = tilepane_wide(pane);  
  h = tilepane_high(pane);  
  return camera_newlockin(camera, x, y, w, h);
} 


/** Sets up the camera so it will stay locked in to the 
extent of the given layer of the tile map. */
Lockin * tilemap_layer_lockin(Tilemap * map, 
                              int layer, 
                              Camera * camera) {
  Tilepane * pane;
  Lockin * result;
  float x, y, w, h;
  if (!map) return NULL;
  pane = tilemap_pane(map, layer);
  return tilepane_lockin(pane, camera);  
}



/* Returns the tile map's area. */
Area * tilemap_area(Tilemap * self) {
  if(!self) return NULL;
  return self->area;
}

/* Returns a thing from this tile map's area. */
Thing * tilemap_thing(Tilemap * self, int index) {
  Area * area = tilemap_area(self);
  return area_thing(area, index);
}

/* Gets the map's grid size from the largest tile layer */
int tilemap_gridwide(Tilemap * self) {
  int wide = 0;
  int index;
  for (index = 0; index < TILEMAP_PANES; index++) {
    int aid = tilepane_gridwide(tilemap_pane(self, index));
    if (aid > wide) {
      wide = aid;
    } 
  }
  return wide;
} 

/* Gets the map's grid size from the largest tile layer */
int tilemap_gridhigh(Tilemap * self) {
  int high = 0;
  int index;
  for (index = 0; index < TILEMAP_PANES; index++) {
    int aid = tilepane_gridhigh(tilemap_pane(self, index));
    if (aid > high) {
      high = aid;
    } 
  }
  return high;
} 

/** Returns the almount of tile panes in this map. */
int tilemap_panes(Tilemap * self) {
  return TILEMAP_PANES;
}

