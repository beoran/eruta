#include "gy.h"

#define GY_INTERN_ONLY
#include "gy_intern.h"

#ifndef GY_TILE_MAXFRAME
#define GY_TILE_MAXFRAME 32
#endif

#ifndef GY_TILE_MAXPROGRAM
#define GY_TILE_MAXPROGRAM 64
#endif

/**
* GyTiledata contains information about the properties of the tiles 
*/
struct GyTiledata_ {
  GyTileinfo *info;
  size_t      size; // How many tiles are actually in use.
};

/** Returns the size of the tile data, or 0 if self or the info is NULL. */
size_t gytiledata_size(GyTiledata * self) {
  if((!self) || (!self->info)) return 0;
  return self->size;
}


/** Cleans up tile data, and empties it.  */
GyTiledata * gytiledata_done(GyTiledata * self) {
  if(!self) return NULL;
  GY_FREE(self->info);
  self->info = NULL; 
  return self; 
}

/** Deallocates tile data*/
GyTiledata * gytiledata_free(GyTiledata * self) {
  gytiledata_done(self);
  GY_FREE(self);
  return NULL;
}

/** Initializes a given tiledata. */
GyTiledata * gytiledata_init(GyTiledata * self, size_t size) {
  if(!self)       return NULL;
  self->info = GY_MALLOC(sizeof(GyTileinfo) * size);
  if(!self->info) return NULL;
  self->size  = size;
  /* Zero out the info. */
  memset(self->info, 0, sizeof(GyTileinfo) * size);
  return self;
}

/** Creates a new tileset with the given size. */
GyTiledata * gytiledata_new(size_t size) {
  GyTiledata * data;
  data   = GY_ALLOCATE(GyTiledata);
  if(!data) return NULL;
  if(gytiledata_init(data, size)) return data;
  // If we get here an error occurred.
  return gytiledata_free(data);
}

/** Index checking for tile info data. */
int gytiledata_indexok(GyTiledata * self, int index) { 
  if(!self)                       return FALSE;
  if(index < 0)                   return FALSE;
  if(index >= self->size)         return FALSE;
  return TRUE;
}

/** Sets the info of a tile in the tiledata. */
GyTiledata * gytiledata_info_(GyTiledata * self, int index, GyTileinfo info) {
  if(!gytiledata_indexok(self, index))  return NULL;
  self->info[index] = info;
  return self;
}

/** Gets the info of a tile in the tiledata. */
GyTileinfo gytiledata_info(GyTiledata * self, int index) {
  if(!gytiledata_indexok(self, index))  return 0;
  return self->info[index];
}

/** Checks if a flag is set on the tile's info. */ 
int gytiledata_isflag(GyTiledata * self, int index, GyTileinfo flag) {
  return ((gytiledata_info(self, index) & flag) == flag);
}

/** Sets a flag on the tile's info. */ 
int gytiledata_setflag(GyTiledata * self, int index, GyTileinfo flag) {
  GyTileinfo old = gytiledata_info(self, index);
  GyTileinfo new = old | flag;
  gytiledata_info_(self, index, new);
  return new;
}

/** Unsets a flag on the tile's info. */ 
int gytiledata_unsetflag(GyTiledata * self, int index, GyTileinfo flag) {
  GyTileinfo old = gytiledata_info(self, index);
  GyTileinfo new = old & (~flag);
  gytiledata_info_(self, index, new);
  return new;
}

/** Toggles a flag on the tile's info. */ 
int gytiledata_toggleflag(GyTiledata * self, int index, GyTileinfo flag) {
  GyTileinfo old = gytiledata_info(self, index);
  GyTileinfo new = old ^ flag;
  gytiledata_info_(self, index, new);
  return new;
}

/** Returns true if the tile with index index is a wall. */
int gytiledata_wall(GyTiledata * self, int index) {
  return gytiledata_isflag(self, index, GyTileWall);
}

/** Toggles the the tile with index index to be a wall, or not. */
int gytiledata_togglewall(GyTiledata * self, int index) {
  return gytiledata_toggleflag(self, index, GyTileWall);
}



