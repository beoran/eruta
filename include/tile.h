#ifndef ERUTA_TILE_H
#define ERUTA_TILE_H

#include "eruta.h"
#include "image.h"

// Default size of tiles is 32x32. Tilesheets should be a multiple of this.
#define TILE_W 32
#define TILE_H 32

struct Tile_;
typedef struct Tile_ Tile;

/** A tile set is a set of many tiles that use the same tile sheet. */
struct Tileset_;
typedef struct Tileset_ Tileset;

/** Cleans up a tileset, and empties it.  */
void tileset_done(Tileset * set);

/** Deallocates a tileset */
void tileset_free(Tileset * set);

/** Initializes a given tileset so it can use the given bitmap as its sheet. */
Tileset * tileset_init(Tileset * set, Image * sheet);

/** Creates a new tileset with the given bitmap as it's tile sheet. */
Tileset * tileset_new(Image * sheet);

/** Initializes a tile to beong to a given tile set. */
Tile * tile_init(Tile * tile, Tileset * set, int index); 

/** Returns a new, unused tile in this tileset and returns it. 
Retuns NULL if the tile could not be added (due to lack of space, etc). */
Tile * tileset_tile(Tileset * set);

/** Gets a tile from a tile set by it's tile id. **/
Tile * tileset_get(Tileset * set, int index);

// Adds a frame to this tile. The frame is an index to a tile in the 
// tile sheet. Otherwise returns the tile itself.
Tile * tile_addframe(Tile * tile, int index);

// Adds an "animation program" step to this tile. The program is consisted of a list 
// of a single byte instructions. Of these bytes, the lower nybble is the opcode 
// and the higher nybble the operand. 
Tile * tile_addanime(Tile * tile, char program);

// Gets the value of the flags of a tile.
int tile_getflag(Tile * tile);

// Sets the flags on a tile. Flags control walkability, etc. 
Tile * tile_setflag(Tile * tile, int flag);

// Rewinds a tile's animations
void tile_rewindanime(Tile * tile);

// Move on to next frame of the animation.  
void tile_nextanime(Tile * tile);

// Updates a tile to animate it
void gytile_update(Tile * tile);

enum TILE_ANIME_OPCODE_ { 
  TILE_ANIME_REWIND = 1,
  TILE_ANIME_NEXT   = 2,
  TILE_ANIME_WAIT   = 3,
};

#define TILE_FRAMES   32
#define TILE_PROGRAMS 32


// Updates all tiles in a tile set so they all get animated.
void tileset_update(Tileset * set);

/** Draw a tile to the current active drawing target at the given coordinates */
void tile_draw(Tile * tile, int x, int y);




#endif



