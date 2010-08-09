#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"

#ifndef GARI_TILE_FRAMES_MAX
#define GARI_TILE_FRAMES_MAX 32
#endif

#ifndef GARI_TILE_PROGRAM_MAX
#define GARI_TILE_PROGRAM_MAX 64
#endif

/**
* A tile set 
*/
struct GariTileset_ {
  GariTile * tiles;
  size_t     size;
  size_t     last;
};

/* 
  A single tile from a tile map. 
  A tile can only hold up to GARI_TILE_FRAMES_MAX tiles pointers in itself.
*/


struct GariTile_ {
  GariImage   * active; 
  GariImage   * frames[GARI_TILE_FRAMES_MAX];
  char        * program[GARI_TILE_PROGRAM_MAX];
  size_t        program_size;
  size_t        frames_size;  
  GariTileset * tileset;
  int           index;
  /** Tileset this stile belongs to + index. */
  int           flags; 
  /** Information about the tile's properties. */
  int           kind;
};


/** Cleans up a tileset, and empties it.  */
void gari_tileset_done(GariTileset * set) {
  if ((set->size > 0) && (set->tiles)) {
    GARI_FREE(set->tiles);
    set->tiles = NULL;
    set->size  = 0;
  }   
}

/** Deallocates a tileset */
void gari_tileset_free(GariTileset * set) {
  gari_tileset_done(set);
  GARI_FREE(set);
}

/** Initializes a given tileset to have a given amount of numbers. */
GariTileset * gari_tileset_init(GariTileset * set, size_t size) {
  if(!set)      return NULL;
  set->last   = 0;
  set->tiles  = GARI_MALLOC(sizeof(GariTile) * size);
  if (!set->tiles)  {
    set->size   = 0; 
    return NULL;
  }
  set->size   = size;
  return set;
}


/** Creates a new tileset with the given size. */
GariTileset * gari_tileset_new(size_t size) {
  GariTileset * set, * result;
  set                         = GARI_ALLOCATE(GariTileset);
  if(!set) return NULL;
  result                      = gari_tileset_init(set, size);
  if(!result) {
     gari_tileset_free(set);
     return NULL;
  }
  return result;
}











