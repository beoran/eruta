#include "eruta.h"

#ifdef COMMENT_

#define BATEXTURE_TILE    "tile"
#define BATEXTURE_PLAYER  "play"
#define BATEXTURE_WEAPON  "arms"
#define BATEXTURE_FOE     "foes"
#define BATEXTURE_ITEM    "item"
#define BAMAP_TILEW       32
#define BAMAP_TILEH       32

/** Loads a texture of the given category and index. Uses the active canvas. */
GyTexture * batexture_load(const char * category, int index) {
  char buf[1024];
  FILE * fout;
  GyImage   * image;
  GyTexture * texture;
  GyCanvas  * canvas  = bamain_get()->canvas;
    
  sprintf(buf, "data/image/%s_%03d.png", category, index);
  image   = gyimage_loadalpha(buf);
  texture = gytexture_new(canvas, image);
  gyimage_free(image);
  return texture;
}

/** Map functionality. */

/* Gets a pointer to the game's current map. */
BaMap * bamap_getcurrent() {
  return &(bamain_get()->map);
}

BaMap* bamap_initempty(BaMap * self) {
  int index;
  self->texture  = NULL;
  self->tiledata = NULL; 
  for(index = 0; index < BAMAP_LAYERS; index++) {
    self->layers[index] = NULL;
  }
  self->space = NULL;
  return self;
}

BaMap* bamap_init(BaMap * self, int textureid, int w, int h) {
  int index, tilesize;
  self->texture   = batexture_load(BATEXTURE_TILE, textureid);
  if(!self->texture) return NULL;
  self->gridw     = w;
  self->gridh     = h;
  self->texture_id= textureid;
  tilesize        = gytexture_w(self->texture) * gytexture_h(self->texture);
  tilesize        = tilesize / (BAMAP_TILEW*BAMAP_TILEH); 
  self->tiledata  = gytiledata_new(tilesize);
  if(!self->tiledata) {  bamap_done(self); return NULL; } 
   
  for(index = 0; index < BAMAP_LAYERS; index++) {
    self->layers[index] = gylayer_new(self->texture, w, h, 32, 32);
  }
  self->space = cpSpaceNew();
  return self;
}

BaMap * bamap_done(BaMap * self) {
  int index;
  if(!self) return NULL;
  for(index = 0; index < BAMAP_LAYERS; index++) {
    gylayer_free(self->layers[index]);
  }  
  cpSpaceFree(self->space);
  gytiledata_free(self->tiledata);
  bamap_initempty(self);
  return self;
}

/** Returns a pointer to the layer at index or NULL if out of range. */
GyLayer * bamap_layer(BaMap * self, int layer) {
  if(!self) return NULL;
  if(layer < 0) return NULL;
  if(layer >= BAMAP_LAYERS) return NULL;
  return self->layers[layer];
}

#define BAMAP_ERROR -2

int bamap_set(BaMap * self, int l, int x, int y, int tile) {
  GyLayer * layer = bamap_layer(self, l);
  if(!layer) return BAMAP_ERROR;
  if(!gylayer_set(layer, x, y, (GyTileindex) tile)) return BAMAP_ERROR;
  // Add physical wall if it's a wall.
  if(bamap_tilewall(self, tile)) {
    bamap_makewall(self, x, y);
  }
  return tile;
}

int bamap_setslab(BaMap * self, int l, int x, int y, int w, int h, int tile) {
  GyLayer * layer = bamap_layer(self, l);
  if(!layer) return BAMAP_ERROR;
  if(!gylayer_setslab(layer, x, y, w, h, (GyTileindex) tile)) return BAMAP_ERROR;
  return tile;
}

int bamap_fill(BaMap * self, int l, int tile) {
  GyLayer * layer = bamap_layer(self, l);
  gylayer_fill(layer, (GyTileindex) tile);
  return tile;
}

int bamap_get(BaMap * self, int l, int x, int y) {
  GyLayer * layer = bamap_layer(self, l);
  return (int) gylayer_get(layer, x, y);
}

/** Sets tile info for given the map and tile. */
int bamap_tileinfo_(BaMap * self, int tile, int info) {
  GyTileinfo ginfo = (GyTileinfo) info;
  if (tile < 0) return -1;
  if (info < 0) return -1;
  if (!self)    return -1;
  gytiledata_info_(self->tiledata, tile, ginfo);
  return info;
}

/** Gets tile info for given the map and tile. */
int bamap_tileinfo(BaMap * self, int tile) {
  if (tile < 0) return -1;
  if (!self)    return -1;
  return gytiledata_info(self->tiledata, tile);
}

/** Checks if a tile index is a wall tile. */
int bamap_tilewall(BaMap * self, int tile) {
  return gytiledata_wall(self->tiledata, tile);
}

/** Makees a physical wall in the chipmunk space of the map at the given 
tile coordinates. Returns the wall shape. */
cpShape * bamap_makewall(BaMap * self, int tx, int ty) {
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



BaMap * bamap_load(lua_State * lua, int index) {
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
  return bamap_getcurrent();
}

GyLayer * gylayer_savefile(GyLayer *layer, int layerid, FILE *fout) {
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

BaMap * bamap_savefile(BaMap * map, FILE * fout) {
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
    int info = bamap_tileinfo(map, index); 
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
    gylayer_savefile(bamap_layer(map, index), index, fout);
  }
  return map;
}


BaMap * bamap_save(BaMap * map, int index) {
  char buf[1024];
  FILE * fout;
  BaMap * res;
  sprintf(buf, "data/script/map_%04d.lua", index);
  fout = fopen(buf, "wt");
  res  = bamap_savefile(map, fout);
  fclose(fout);
  return res;
}

void bamap_draw(BaMap * map, GyWindow * window, int x, int y) {
  int index;
  for(index = 0; index < BAMAP_LAYERS; index++) {
    gylayer_draw(bamap_layer(map, index), window, x, y);
  }
}


#endif