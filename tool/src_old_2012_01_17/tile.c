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
* A tile set 
*/
struct GyTileset_ {
  GyTile * tiles;
  size_t     size;
  size_t     last;
};

/** 
*  A single tile from a tile map. 
*  A tile can only hold up to GY_TILE_FRAMES_MAX tiles pointers in itself.
*/


struct GyTile_ {
  GyImage   * active; 
  GyImage   * frames[GY_TILE_MAXFRAME];
  char          program[GY_TILE_MAXPROGRAM];  
  size_t        proglen;
  size_t        framlen;  
  GyTileset * set;
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
void gytileset_done(GyTileset * set) {
  if ((set->size > 0) && (set->tiles)) {
    GY_FREE(set->tiles);
    set->tiles = NULL;
    set->size  = 0;
  }   
}

/** Deallocates a tileset */
void gytileset_free(GyTileset * set) {
  gytileset_done(set);
  GY_FREE(set);
}

/** Initializes a given tileset to have a given amount of numbers. */
GyTileset * gytileset_init(GyTileset * set, size_t size) {
  if(!set)      return NULL;
  set->last   = 0;
  set->tiles  = GY_MALLOC(sizeof(GyTile) * size);
  if (!set->tiles)  {
    set->size   = 0; 
    return NULL;
  }
  set->size   = size;
  return set;
}


/** Creates a new tileset with the given size. */
GyTileset * gytileset_new(size_t size) {
  GyTileset * set, * result;
  set                         = GY_ALLOCATE(GyTileset);
  if(!set) return NULL;
  result                      = gytileset_init(set, size);
  if(!result) {
     gytileset_free(set);
     return NULL;
  }
  return result;
}

/** Initializes a tile to beong to a given tile set. */
GyTile * gytile_init(GyTile * tile, GyTileset * set, int index) {
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
GyTile * gytileset_tile(GyTileset * set) {
  GyTile * tile = NULL;
  if(!set) return NULL;
  if (set->last >= set->size) return NULL; // no more space
  tile = set->tiles + set->last; // Get free tile from end of tile array.  
  gytile_init(tile, set, set->last);  
  set->last++;
  return tile;
} 

/** Gets a tile from a tile set by it's tile id. **/ 
GyTile * gytileset_get(GyTileset * set, int index) {
  if(!set) return NULL;
  if (index < 0) return NULL;
  if (set->last >= ((size_t)index)) { // tile id out of bounds
    return NULL;
  }
  return set->tiles + index;
}


// Adds an image to this tile. May return NULL if not enough space, etc. 
// Otherwise returns the tile itself. 
GyTile * gytile_addframe(GyTile * tile, GyImage * image) {
  if((!tile) ||(!image)) return NULL;
  if(tile->framlen >= GY_TILE_MAXFRAME) return NULL;
  tile->frames[tile->framlen] 	= image;
  // Make last added tile acive.
  tile->active 			= tile->frames[tile->framlen];   
  tile->framlen++;  
  return tile;
}

// Adds an "animation program" step to this tile. The program is consisted of a list 
// of a single byte instructions. Of these bytes, he lower nybble is the opcode 
// and the higher nybble the operand. 
GyTile * gytile_addanime(GyTile * tile, char program) {
  if(!tile) return NULL;
  if(tile->proglen >= GY_TILE_MAXPROGRAM) return NULL;
  tile->program[tile->proglen] = program;
  // Make last added tile acive.  
  tile->proglen++;  
  return tile;
}

// Gets the value of the flags of a tile.
int gytile_getflag(GyTile * tile) {
  if(!tile) return 0;
  return tile->flags;
}

// Sets the flags on a tile.
GyTile * gytile_setflag(GyTile * tile, int flag) {
  if(!tile) return NULL;
  tile->flags = flag;
  return tile;
}

// Rewinds a tile's animations
void gytile_rewindanime(GyTile * tile) {
  if (!tile) return;
  tile->anime  = 0;
  tile->offset = 0;
  tile->active = tile->frames[tile->offset];
}

// Move on to next frame of the animation.  
void gytile_nextanime(GyTile * tile) {
  if (!tile) return;
  tile->anime++;
  tile->offset++;
  tile->active = tile->frames[tile->offset];
}

// Updates a tile to animate it
void gytile_update(GyTile * tile) {
  char program, opcode, operand;
  if (!tile) return;
  if (tile->proglen < 1) return; // no anim program means no animation;
  // next animation step. 
  program = tile->program[tile->anime];
  opcode  = program &  31;
  operand = program >> 5;
  switch (opcode) {
    case GyAnimeRewind: // Back to first tile
      gytile_rewindanime(tile); // Rewind animation.
      break;
    case GyAnimeNext: // Move to next tile in animation
      gytile_nextanime(tile);
      break;
    default:  // Wait, only increasing the anmation index
      tile->anime++;
    break;	
  }  
 // Rewind active tile index if too far. Yes, this is defensive programming, 
 // but it's better like this. :)
  if (tile->offset >= tile->framlen) {
      gytile_rewindanime(tile);
  } 
  // also rewind anime index if too far.       
  if (tile->anime >= tile->proglen) {   
    tile->anime = 0;
  }
}

// Updates all tiles in a tile set so they all get animated.
void gytileset_update(GyTileset * set) {
  int index;
  if (!set) return;
  if (!set->tiles) return;
  for (index = 0; index < set->size ; index++) {
    GyTile * tile = gytileset_get(set, index); 
    gytile_update(tile);
  }  
} 







