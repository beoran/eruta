#include "mem.h"
#include "tile.h"

#ifndef TILE_MAXFRAME
#define TILE_MAXFRAME 32
#endif

#ifndef TILE_MAXPROGRAM
#define TILE_MAXPROGRAM 64
#endif

/**
* A tile set 
*/
struct Tileset_ {
  Tile     * tiles;
  Image    * sheet;
  size_t     size;
  size_t     last;
  int        w;
  int        h;
};

/** 
* A single tile from a tile map. 
* A tile can only hold up to TILE_FRAMES tiles pointers in itself.
*/
struct Tile_ {
  int           frames[TILE_FRAMES];
  char          program[TILE_PROGRAMS];
  size_t        proglen;
  size_t        framlen;
  Tileset     * set;
  int           index;
  /** Tileset this tile belongs to + index. */
  int           flags;
  /** Information about the tile's properties. */
  int           kind;
  /** Index of currently active animation pointer for this tile. */
  int           anime;
  int           offset;
  /** Index of currently active image pointer for this tile. */
  int           active;
  /** Sub-position in the sheet of the currently active frame. */
  Point         now;
};

/* NOTE: Tiles could be implemented using sub bitmaps as they seem to be
* slightly faster if they are preallocated. however the speed gain would
* be around 2%, so it's not a priority yet. It could simplify some of
* the code, though, but I'll use sub-bitmaps for spites first.
*/


/** Cleans up a tileset, and empties it.  */
void tileset_done(Tileset * set) {
  if ((set->size > 0) && (set->tiles)) {
    mem_free(set->tiles);
    set->tiles = NULL;
    set->size  = 0;
    set->w      = -1;
    set->h      = -1;
  }
}

/** Deallocates a tileset */
void tileset_free(Tileset * set) {
  tileset_done(set);
  mem_free(set);
}

/** Initializes a given tileset with a given bitmap tile sheet */
Tileset * tileset_init(Tileset * set, Image * sheet) {
  if(!set)      return NULL;
  set->sheet    = sheet;
  if(!set->sheet)    {
    set->w      = -1;
    set->h      = -1;
    set->size   = 0;
    set->tiles  = NULL;
    return NULL;
  }
  // alow re-init
  if (set->tiles) {
    tileset_done(set);
  } 
  set->w        = image_w(set->sheet);
  set->h        = image_h(set->sheet);
  set->size     = (set->w / 32) * (set->h / 32); 
  set->last     = 0;
  set->tiles    = mem_alloc(sizeof(Tile) * set->size);
  if (!set->tiles) {
    set->w      = -1;
    set->h      = -1;
    set->size   = 0;
    return NULL;
  }
  return set;
}

/** Creates a new tileset with the given tile sheet image. */
Tileset * tileset_new(Image * sheet) {
  Tileset * set, * result;
  set                         = STRUCT_ALLOC(Tileset);
  if(!set) return NULL;
  result                      = tileset_init(set, sheet);
  if(!result) {
     tileset_free(set);
     return NULL;
  }
  return result;
}

/*Macros thta calculate the position of a tile in a tile set's sheet. */
#define TILE_SHEET_Y(TILE, SET)\
        ((TILE->active * TILE_H) / ((SET)->w))

#define TILE_SHEET_X(TILE, SET)\
        ((TILE->active * TILE_W) % ((SET)->w))
  
/** Recalculates the tile's position (now) in it's tile set. */
Tile * tile_recalculate(Tile * tile) {
  int x, y;
  if(!tile) return NULL;
  if(!tile->set) return NULL;
  x = TILE_SHEET_X(tile, tile->set);
  y = TILE_SHEET_Y(tile, tile->set);
  tile->now = point(x, y); 
  return tile;
}

/** Initializes a tile to belong to a given tile set. */
Tile * tile_init(Tile * tile, Tileset * set, int index) {
  if(!tile) return NULL;
  if(!set) return NULL;
  tile->index   = index;
  tile->set     = set;
  tile->proglen = 0;
  tile->framlen = 0;
  tile->anime   = 0;
  tile->offset  = 0; 
  tile->active  = index;
  tile_recalculate(tile);
  return tile;
}

/** Returns a new, unused tile in this tileset and returns it. 
Retuns NULL if the tile could not be added (due to lack of space, etc). */
Tile * tileset_tile(Tileset * set) {
  Tile * tile = NULL;
  if(!set) return NULL;
  if (set->last >= set->size) return NULL; // no more space
  tile = set->tiles + set->last; // Get free tile from end of tile array.  
  tile_init(tile, set, set->last);
  set->last++;
  return tile;
} 

/** Gets a tile from a tile set by it's tile id. **/ 
Tile * tileset_get(Tileset * set, int index) {
  if(!set) return NULL;
  if (index < 0) return NULL;
  if (set->last >= ((size_t)index)) { // tile id out of bounds
    return NULL;
  }
  return set->tiles + index;
}


// Adds an image to this tile. May return NULL if not enough space, etc. 
// Otherwise returns the tile itself. 
Tile * tile_addframe(Tile * tile, int index) {
  if((!tile)) return NULL;
  if(tile->framlen >= TILE_FRAMES) return NULL;
  tile->frames[tile->framlen] 	= index;
  // Make last added frame active.
  tile->active = tile->frames[tile->framlen];
  tile->framlen++;
  return tile;
}

// Adds an "animation program" step to this tile. The program is consisted of a list 
// of a single byte instructions. Of these bytes, he lower nybble is the opcode 
// and the higher nybble the operand. 
Tile * tile_addanime(Tile * tile, char program) {
  if(!tile) return NULL;
  if(tile->proglen >= TILE_PROGRAMS) return NULL;
  tile->program[tile->proglen] = program;
  tile->proglen++;
  return tile;
}

// Gets the value of the flags of a tile.
int tile_getflag(Tile * tile) {
  if(!tile) return 0;
  return tile->flags;
}

// Sets the flags on a tile.
Tile * tile_setflag(Tile * tile, int flag) {
  if(!tile) return NULL;
  tile->flags = flag;
  return tile;
}

// Rewinds a tile's animations
void tile_rewindanime(Tile * tile) {
  if (!tile) return;
  tile->anime  = 0;
  tile->offset = 0;
  tile->active = tile->frames[tile->offset];
}

// Move on to next frame of the animation.  
void tile_nextanime(Tile * tile) {
  if (!tile) return;
  tile->anime++;
  tile->offset++;
  tile->active = tile->frames[tile->offset];
}

// Updates a tile to animate it
void tile_update(Tile * tile) {
  char program, opcode, operand;
  if (!tile) return;
  if (tile->proglen < 1) return; // no anim program means no animation;
  // next animation step. 
  program = tile->program[tile->anime];
  opcode  = program &  31;
  operand = program >> 5;
  switch (opcode) {
    case TILE_ANIME_REWIND: // Back to first tile
      tile_rewindanime(tile); // Rewind animation.
      break;
    case TILE_ANIME_NEXT: // Move to next tile in animation
      tile_nextanime(tile);
      break;
    default:  // Wait, only increasing the anmation index
      tile->anime++;
    break;	
  }  
 // Rewind active tile index if too far. Yes, this is defensive programming, but it's better like this. :)
  if (tile->offset >= tile->framlen) {
      tile_rewindanime(tile);
  } 
  // Also rewind anime index if too far.
  if (tile->anime >= tile->proglen) {   
    tile->anime = 0;
  }
  // Finally recalculate tile position.
  tile_recalculate(tile);
}

// Updates all tiles in a tile set so they all get animated.
void tileset_update(Tileset * set) {
  int index;
  if (!set) return;
  if (!set->tiles) return;
  for (index = 0; index < set->size ; index++) {
    Tile * tile = tileset_get(set, index); 
    tile_update(tile);
  }  
} 


        
/** Draw a tile to the current active drawing target at the
given coordinates */
void tile_draw(Tile * tile, int x, int y) {
  Tileset * set = tile->set;
  Image * sheet = set->sheet;
  float dx      = (float) x;
  float dy      = (float) y; 
  float sx      = (float) tile->now.x;
  float sy      = (float) tile->now.y;
  float sw      = (float) TILE_W;
  float sh      = (float) TILE_H;
  // printf("%f %f\n", sx, sy);
  al_draw_bitmap_region(sheet, sx, sy, sw, sh, dx, dy, 0);
  // al_draw_bitmap(sheet, dx, dy, 0);
}







