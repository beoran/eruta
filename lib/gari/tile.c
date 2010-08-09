#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"

#ifndef GARI_TILE_MAXFRAME
#define GARI_TILE_MAXFRAME 32
#endif

#ifndef GARI_TILE_MAXPROGRAM
#define GARI_TILE_MAXPROGRAM 64
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
  GariImage   * frames[GARI_TILE_MAXFRAME];
  char          program[GARI_TILE_MAXPROGRAM];  
  size_t        proglen;
  size_t        framlen;  
  GariTileset * set;
  int           index;
  /** Tileset this stile belongs to + index. */
  int           flags; 
  /** Information about the tile's properties. */
  int           kind;
  /** Index of currently active animation pointer for this tile. */
  int 		anime;
  /** Index of currently active image pointer for this tile. */
  int 		offset;
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

/** Initializes a tile to beong to a given tile set. */
GariTile * gari_tile_init(GariTile * tile, GariTileset * set, int index) {
  if(!tile) return NULL;
  if(!set) return NULL;
  if(index < 0) return NULL;
  tile->index 	= index;
  tile->set 	= set;
  tile->proglen = 0;
  tile->framlen = 0;
  tile->anime   = 0;
  tile->offset  = 0; 
  tile->active  = NULL;
  return tile;
}

/** Returns a new, unused tile in this tileset and returns it. 
Retuns NULL if the tile could not be added (due to lack of space, etc). */
GariTile * gari_tileset_tile(GariTileset * set) {
  GariTile * tile = NULL;
  if(!set) return NULL;
  if (set->last >= set->size) return NULL; // no more space
  tile = set->tiles + set->last; // Get free tile from end of tile array.  
  gari_tile_init(tile, set, set->last);  
  set->last++;
  return tile;
} 

// Gets a tile from a tile set by it's number. 
GariTile * gari_tileset_get(GariTileset * set, int index) {
  if(!set) return NULL;
  if (index < 0) return NULL;
  if (set->last >= ((size_t)index)) { // tile out of bounds
    return NULL;
  }
  return set->tiles + index;
}


// Adds an image to this tile. May return NULL if not enough space, etc. 
// Otherwise returns the tile itself. 
GariTile * gari_tile_addframe(GariTile * tile, GariImage * image) {
  if((!tile) ||(!image)) return NULL;
  if(tile->framlen >= GARI_TILE_MAXFRAME) return NULL;
  tile->frames[tile->framlen] 	= image;
  // Make last added tile acive.
  tile->active 			= tile->frames[tile->framlen];   
  tile->framlen++;  
  return tile;
}

// Adds an "animation program" step to this tile. The program is consisted of a list 
// of a single byte instructions. Of these bytes, he lower nybble is the opcode 
// and the higher nybble the operand. 
GariTile * gari_tile_addanime(GariTile * tile, char program) {
  if(!tile) return NULL;
  if(tile->proglen >= GARI_TILE_MAXPROGRAM) return NULL;
  tile->program[tile->proglen] = program;
  // Make last added tile acive.  
  tile->proglen++;  
  return tile;
}

// Gets the value of the flags of a tile.
int gari_tile_getflag(GariTile * tile) {
  if(!tile) return 0;
  return tile->flags;
}

// Sets the flags on a tile.
GariTile * gari_tile_setflag(GariTile * tile, int flag) {
  if(!tile) return NULL;
  tile->flags = flag;
  return tile;
}

// Rewinds a tile's animations
void gari_tile_rewindanime(GariTile * tile) {
  if (!tile) return;
  tile->anime  = 0;
  tile->offset = 0;
  tile->active = tile->frames[tile->offset];
}

// Move on to next frame of the animation.  
void gari_tile_nextanime(GariTile * tile) {
  if (!tile) return;
  tile->anime++;
  tile->offset++;
  tile->active = tile->frames[tile->offset];
}

// Updates a tile to animate it
void gari_tile_update(GariTile * tile) {
  char program, opcode, operand;
  if (!tile) return;
  if (tile->proglen < 1) return; // no anim program means no animation;
  // next animation step. 
  program = tile->program[tile->anime];
  opcode  = program &  31;
  operand = program >> 5;
  switch (opcode) {
    case GariAnimeRewind: // Back to first tile
      gari_tile_rewindanime(tile); // Rewind animation.
      break;
    case GariAnimeNext: // Move to next tile in animation            
      gari_tile_nextanime(tile);
      break;
    default:  // Wait, only increasing the anmation index
      tile->anime++;
    break;	
  }  
 // Rewind active tile index if too far. Yes, this is defensive programming, 
 // but it's better like this. :)
  if (tile->offset >= tile->framlen) {
      gari_tile_rewindanime(tile);
  } 
  // also rewind anime index if too far.       
  if (tile->anime >= tile->proglen) {   
    tile->anime = 0;
  }
}

// Updates all tiles in a tile set so they all get animated.
void gari_tileset_update(GariTileset * set) {
  int index;
  if (!set) return;
  if (!set->tiles) return;
  for (index = 0; index < set->size ; index++) {
    GariTile * tile = gari_tileset_get(set, index); 
    gari_tile_update(tile);
  }  
} 







