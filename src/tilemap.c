#include "image.h"
#include "tile.h"
#include "tilepane.h"

#define TILEMAP_PANES 4

/** A tile map is a game map that uses tiled layers for it's 
display and physics. */
struct Tilemap_ {
  Image     * texture;
  int         textureid;
  int         gridw;
  int         gridh;
  Tileset   * set;
  Tilepane  * panes[TILEMAP_PANES];
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
    tilepane_free(self->layers[index]);
  }  
  // cpSpaceFree(self->space);
  tileset_free(self->set);
  // tilemap_initempty(self);
  return self;
}


/** Initializes a tile map */
Tilemap* tilemap_init(Tilemap * self, int textureid, int w, int h) {
  int index, tilesize;
  self->texture   = batexture_load(TEXTURE_TILE, textureid);
  if(!self->texture) return NULL;
  self->gridw     = w;
  self->gridh     = h;
  self->textureid = textureid;
  self->set       = tileset_new(self->texture);
  if(!self->set) {  tilemap_done(self); return NULL; }
   
  for(index = 0; index < TILEMAP_PANES; index++) {
    self->panes[index] = tilepane_new(self->texture, w, h);
  }
  self->space = cpSpaceNew();
  return self;
}


/** Returns a pointer to the pane at index or NULL if out of range. */
Tilepane * tilemap_layer(Tilemap * self, int layer) {
  if(!self) return NULL;
  if(layer < 0) return NULL;
  if(layer >= TILEMAP_PANES) return NULL;
  return self->panes[layer];
}

/** Sets a tile in the tile map to the given tile. */
Tilemap * tilemap_settile(TileMap * self, int l, int x, int y, Tile * tile) {
  Tilepane * pane = tilemap_layer(self, l);
  if(!layer) return NULL;
  if(!tilepane_set(layer, x, y, tile)) return NULL;
  // TODO: Add physical wall if it's a wall.
  /*
  if(tilemap_tilewall(self, tile)) {
    tilemap_makewall(self, x, y);
  }
  */
  return tile;
}

/** Sets a tile in the tile map to the tile with the given index. */
Tilemap * tilemap_settile(TileMap * self, int l, int x, int y, int index) {
  Tilepane * pane = tilemap_layer(self, l);
  Tile     * tile = NULL;
  if(!layer) return NULL;
  tile            = tilepane_setindex(layer, x, y, index);
  if(!tile) return NULL;
  // TODO: Add physical wall if it's a wall.
  /*
  if(tilemap_tilewall(self, tile)) {
    tilemap_makewall(self, x, y);
  }
  */
  return tile;
}


#ifdef COMMENT_


int tilemap_set(TileMap * self, int l, int x, int y, int tile) {
  Tilepane * layer = tilemap_layer(self, l);
  if(!layer) return BAMAP_ERROR;
  if(!gylayer_set(layer, x, y, (GyTileindex) tile)) return BAMAP_ERROR;
  // Add physical wall if it's a wall.
  if(tilemap_tilewall(self, tile)) {
    tilemap_makewall(self, x, y);
  }
  return tile;
}



#define BAMAP_ERROR -2


int tilemap_setslab(TileMap * self, int l, int x, int y, int w, int h, int tile) {
  Tilepane * layer = tilemap_layer(self, l);
  if(!layer) return BAMAP_ERROR;
  if(!gylayer_setslab(layer, x, y, w, h, (GyTileindex) tile)) return BAMAP_ERROR;
  return tile;
}

int tilemap_fill(TileMap * self, int l, int tile) {
  Tilepane * layer = tilemap_layer(self, l);
  gylayer_fill(layer, (GyTileindex) tile);
  return tile;
}

int tilemap_get(TileMap * self, int l, int x, int y) {
  Tilepane * layer = tilemap_layer(self, l);
  return (int) gylayer_get(layer, x, y);
}

/** Sets tile info for given the map and tile. */
int tilemap_tileinfo_(TileMap * self, int tile, int info) {
  GyTileinfo ginfo = (GyTileinfo) info;
  if (tile < 0) return -1;
  if (info < 0) return -1;
  if (!self)    return -1;
  gytiledata_info_(self->tiledata, tile, ginfo);
  return info;
}

/** Gets tile info for given the map and tile. */
int tilemap_tileinfo(TileMap * self, int tile) {
  if (tile < 0) return -1;
  if (!self)    return -1;
  return gytiledata_info(self->tiledata, tile);
}

/** Checks if a tile index is a wall tile. */
int tilemap_tilewall(TileMap * self, int tile) {
  return gytiledata_wall(self->tiledata, tile);
}

/** Makees a physical wall in the chipmunk space of the map at the given 
tile coordinates. Returns the wall shape. */
cpShape * tilemap_makewall(TileMap * self, int tx, int ty) {
  cpFloat x     = (cpFloat)(tx * BAMAP_TILEW);
  cpFloat y     = (cpFloat)(ty * BAMAP_TILEW);
  cpFloat w     = (cpFloat)(BAMAP_TILEW);
  cpFloat h     = (cpFloat)(BAMAP_TILEH);
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



TileMap * tilemap_load(lua_State * lua, int index) {
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

Tilepane * gylayer_savefile(Tilepane *layer, int layerid, FILE *fout) {
  int xx, yy, gw, gh;
  gw = gylayer_gridwide(layer);
  gh = gylayer_gridhigh(layer);
  for(yy = 0; yy < gh; yy++) {
    fprintf(fout, "mapsetrow{ %d, %d ", layerid, yy);    
    for(xx = 0; xx < gw; xx++) {
      int tile = (int) gylayer_get(layer, xx, yy);
      fprintf(fout, ", %d", tile);
    }
    fprintf(fout, "}\n");
  }
  return layer;
}

TileMap * tilemap_savefile(TileMap * map, FILE * fout) {
  int index, tds;
  if(!fout) return NULL;
  fprintf(fout, "-- Automatical save of map, DO NOT HAND EDIT.\n");
  fprintf(fout, "-- In stead, edit mex_xxxx.lua .\n--\n");  
  fprintf(fout, "-- Initialize map.\n");
  fprintf(fout, "map = mapinit(%d, %d, %d)\n", 
          map->texture_id, map->gridw, map->gridh);
  /* save tile info */
  fprintf(fout, "-- Tile info.\n");
  tds = gytiledata_size(map->tiledata);
  fprintf(fout, "maptiledata { ");
  for (index = 0; index < tds; index++) {
    int info = tilemap_tileinfo(map, index);
    if(info) { 
      fprintf(fout, "[%d] = %d; ", index, info);
      //fprintf(fout, "maptileinfo(%d, %d)\n", index, info);
    } 
  }
  fprintf(fout, "}\n");
  fprintf(fout, "-- Layer data.\n");        
  for(index = 0; index < BAMAP_LAYERS; index++) {
    fprintf(fout, "-- Layer nr %d.\n", index);
    fprintf(fout, "mapfill(%d, %d)\n", index, -1);
    gylayer_savefile(tilemap_layer(map, index), index, fout);
  }
  return map;
}


TileMap * tilemap_save(TileMap * map, int index) {
  char buf[1024];
  FILE * fout;
  TileMap * res;
  sprintf(buf, "data/script/map_%04d.lua", index);
  fout = fopen(buf, "wt");
  res  = tilemap_savefile(map, fout);
  fclose(fout);
  return res;
}

void tilemap_draw(TileMap * map, GyWindow * window, int x, int y) {
  int index;
  for(index = 0; index < BAMAP_LAYERS; index++) {
    gylayer_draw(tilemap_layer(map, index), window, x, y);
  }
}


#endif