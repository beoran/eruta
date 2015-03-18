
// #include <mxml.h>
#include "mem.h"
#include "image.h"
#include "tilemap.h"
#include "dynar.h"
#include "monolog.h"

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
Tilemap* tilemap_init(Tilemap * self, Tileset * set, int w, int h, Area * unused__) {
  int index;
  
  (void) unused__;
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
  return self;
}

/** Frees the tile map and initializes it. */
Tilemap * tilemap_free(Tilemap * map) {
  tilemap_done(map);
  mem_free(map);
  return NULL;
}

/** Allocates a new tile map and initializes it. */
Tilemap * tilemap_new(Tileset * set, int w, int h, Area * unused__ ) {
  Tilemap * map = STRUCT_ALLOC(Tilemap);
  (void) unused__;
  if(!tilemap_init(map, set, w, h, NULL)) {
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


/** Returns the tile in the tile map in the given layer at the given tile coords. */
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


/** Sets the drawing flags of the tile at (x, y) in layer l. */
int tilemap_set_flags(Tilemap * self, int l, int x, int y, int flags) {
  Tilepane * pane = tilemap_pane(self, l);
  if (!pane) return -1;
  return tilepane_set_flags(pane, x, y, flags);
}

/** Gets the drawing flags of the tile at (x, y) in layer l. */
int tilemap_get_flags(Tilemap * self, int l, int x, int y) {
  Tilepane * pane = tilemap_pane(self, l);
  if (!pane) return -1;
  return tilepane_get_flags(pane, x, y);
}





/** Draws a tile map. */
void tilemap_draw(Tilemap * map, Camera * camera) {
  int index;
  Tilepane * pane;
  Tilepane * floor = NULL;

  /* XXX the blends and the shadows don't mix well, perhaps draw shadows
   * first???
   */
  for(index  = 0; index < TILEMAP_PANES; index++) {
    pane     = tilemap_pane(map, index);
    if (pane) {
      tilepane_draw(pane, camera);
      if (index == 0) {
        tilepane_draw_blends(pane, camera);
        floor = pane;
      } else if (index > 0) {
        tilepane_draw_shadows_of(pane, floor, camera);
      }
    } else {
      LOG_WARNING("Pane missing: %d", index);
    }
  }
}



/** Draws a layer in the tile map with the given index, 
 without blends nor shadows. */
void tilemap_draw_layer_tiles(Tilemap * map, Camera * camera, int layer) {
  Tilepane * pane;
  pane     = tilemap_pane(map, layer);
  if (pane) {
    tilepane_draw(pane, camera);
  }
}


/** Draws the blends in a layer in the tile map with the given index, 
 if it exists. Otherwise does nothing. */
void tilemap_draw_layer_blends(Tilemap * map, Camera * camera, int layer) {
  Tilepane * pane;
  pane     = tilemap_pane(map, layer);
  if (pane) {
    tilepane_draw_blends(pane, camera);
  }
}


/** Draws the shadows a given layer in a tile map casts if it exists
 *  and isn't equal to 0.
 * Otherwise does nothing. */
void tilemap_draw_layer_shadows(Tilemap * map, Camera * camera, int layer) {
  Tilepane * pane;
  Tilepane * floor;
  if (layer < 1) return ;
  pane     = tilemap_pane(map, layer);
  if (!pane) return;
  floor = tilemap_pane(map, layer-1);
  tilepane_draw_shadows_of(pane, floor, camera);
}

/** Draws a layer in the tile map with the given index, 
 if it exists. Otherwise does nothing. */
void tilemap_draw_layer(Tilemap * map, Camera * camera, int layer) {
  /* Shadows must be drawn before blends otherwise both won't look good. */
  tilemap_draw_layer_tiles(   map, camera, layer);
  tilemap_draw_layer_shadows( map, camera, layer);
  tilemap_draw_layer_blends(  map, camera, layer);
}


/** Updates the tile map. Currently this animates the tiles. */
void tilemap_update(Tilemap * map, double dt) {
  tileset_update(map->set, dt);
}

/** Adds a dynamic thing of the given type to the tile map's area */
Thing * tilemap_addthing(Tilemap * self, int index, int kind, int x, int y, int z, 
                         int w, int h) {
  return area_newdynamic(self->area, index, kind, x, y, z, w, h);
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
  (void) self;
  return TILEMAP_PANES;
}


/** Sets up automatic blending for this tile map. This is called automatically 
  after loading the Tilemap */
bool tilemap_init_blend(Tilemap * self) {
  bool res = true;
  bool aid;
  int index;
  for (index = 0; index < TILEMAP_PANES; index++) {
    aid = tilepane_init_blend(tilemap_pane(self, index), index);
    res = aid && res;
  }  
  return res;
}

/* Returns the tileset used for this tile map.  */
Tileset * tilemap_tileset(Tilemap * me) {
  if (!me) return NULL;
  return me->set;
}

/** Returns the firstgid for the tileset of this map, 
 * or negative on error */
/* Returns the tileset used for this tile map.  */
int tilemap_firstgid(Tilemap * me) {
  Tileset * set = tilemap_tileset(me);
  if (!set) return -1;
  return tileset_firstgid(set);
} 



#ifdef SEPARATE_TILEMAP_LOADER

/* Amount of tilemaps that can be loaded at oce. */
#ifndef ERUTA_TILEMAPS_MAX
#define ERUTA_TILEMAPS_MAX 16
#endif


/* "Globally" loaded tile maps. Allow up to ERUTA_TILEMAPS_MAX tile maps to be 
 * loaded at the same time. */
static Tilemap * tilemaps[ERUTA_TILEMAPS_MAX];

static int active_tilemap = -1;
/* Currently active tile map, if any. Negative means, none active. */

int tilemaps_max(void) {
  return ERUTA_TILEMAPS_MAX;
}


int tilemaps_out_of_bounds(int index) {
  if (index < 0)                return TRUE;
  if (index >tilemaps_max())    return TRUE;
  return false;
}

Tilemap * tilemaps_put_raw(int index, Tilemap * map) {
  if(tilemaps_out_of_bounds(index)) return NULL;
  return tilemaps[index] = map;
}

/* Puts map at index, unloading any previously loaded map. */
Tilemap * tilemaps_put(int index, Tilemap * map) {
  if(tilemaps_out_of_bounds(index)) return NULL;
  if(tilemaps[index]) tilemap_free(tilemaps[index]);
  return tilemaps[index] = map;
}

int tilemaps_init(void) {
  int index;
  for(index = 0; index < tilemaps_max(); index ++) {
    tilemaps_put_raw(index, NULL);
  }
}

Tilemap * tilemaps_get(int index) {
  if(tilemaps_out_of_bounds(index)) return NULL;
  return tilemaps[index];
}

/* Deallocates all loaded tile maps. */
int tilemaps_done(void) {
  int index;
  for(index = 0; index < tilemaps_max(); index ++) {
    tilemaps_put(index, NULL);
  }  
}



#endif





