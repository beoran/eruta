#ifndef ERUTA_TILE_H
#define ERUTA_TILE_H

#include "eruta.h"
#include "image.h"

// Default size of tiles is 32x32. Tilesheets should be a multiple of this.
#define TILE_W 32
#define TILE_H 32

enum TILE_ANIME_OPCODE_ {
  TILE_ANIME_REWIND = 1,
  TILE_ANIME_NEXT   = 2,
  TILE_ANIME_WAIT   = 3,
};

/* Tile flags. */ 
enum TILE_FLAGS_ {
  TILE_WALL         = 1 << 1,
  TILE_WATER        = 1 << 2,
  TILE_LEDGE        = 1 << 3,
  TILE_STAIR        = 1 << 4,
  TILE_PUSH         = 1 << 5,
  TILE_NORTH        = 1 << 6,
  TILE_SOUTH        = 1 << 7,
  TILE_EAST         = 1 << 8,
  TILE_WEST         = 1 << 9,
};


#define TILE_FRAMES   32
#define TILE_PROGRAMS 32

#include "tile_proto.h"



#endif



