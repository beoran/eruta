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

#define TILE_FRAMES   32
#define TILE_PROGRAMS 32

#include "tile_proto.h"



#endif



