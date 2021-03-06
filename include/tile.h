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
  TILE_WALL         = 1 << 0,
  TILE_WATER        = 1 << 1,
  TILE_LEDGE        = 1 << 2,
  TILE_STAIR        = 1 << 3,
  TILE_PUSH         = 1 << 4,
  TILE_NORTH        = 1 << 5,
  TILE_SOUTH        = 1 << 6,
  TILE_EAST         = 1 << 7,
  TILE_WEST         = 1 << 8,  
  TILE_UP           = 1 << 9,
  TILE_DOWN         = 1 << 10,
  TILE_ICE          = 1 << 11,
};


#define TILE_FRAMES   32
#define TILE_PROGRAMS 32

typedef struct Tileset_ Tileset;
typedef struct Tile_ Tile;


void tileset_done (Tileset * set );

void tileset_free (Tileset * set );

int tileset_size (Tileset * set );
int tileset_firstgid(Tileset * set);

/** Initializes a given tileset with a given bitmap tile sheet and firstgid */
Tileset * tileset_init(Tileset * set, Image * sheet, int firstgid); 

Tileset * tileset_new(Image * sheet, int firstgid );

Tile * tile_recalculate (Tile * tile );

Tile * tile_init (Tile * tile , Tileset * set , int index );

Tile * tileset_get (Tileset * set , int index );

Tile * tile_anim_ (Tile * tile , int anim );

int tile_anim (Tile * tile );

Tile * tile_wait_ (Tile * tile , int wait );

int tile_wait (Tile * tile );

int tile_flags (Tile * tile );

Tile * tile_flags_ (Tile * tile , int flags );

Tile * tile_setflag (Tile * tile , int flag );

Tile * tile_unflag (Tile * tile , int flag );

int tile_isflag (Tile * tile , int flag );

Tile * tile_property_ (Tile * tile , char * property );

void tile_rewindanime (Tile * tile );

void tile_update (Tile * tile , double dt );

void tileset_update (Tileset * set , double dt );

void tile_draw (Tile * tile , int x , int y, int drawflags);

int tile_index (Tile * tile );

int tile_kind (Tile * tile );

void tile_draw_masked_to
(Image * result, Tile * tile, Image * mask, float angle, int mask_flags);

int tile_blend_(Tile * tile , int priority);
int tile_blend(Tile * tile );
int tile_blend_mask(Tile * tile);
int tile_blend_mask_(Tile * tile, int mask);

int tile_light_(Tile * tile , int value);
int tile_light(Tile * tile );
int tile_light_mask(Tile * tile);
int tile_light_mask_(Tile * tile, int mask);

int tile_shadow_(Tile * tile , int value);
int tile_shadow(Tile * tile );
int tile_shadow_mask(Tile * tile);
int tile_shadow_mask_(Tile * tile, int mask);

Tile * tile_add_animation_frame(Tile * tile, int index, double duration);



#endif


