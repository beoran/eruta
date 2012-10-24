#include "eruta.h"

#include "area.h"
#include "mem.h"

/** Scale factor. */
#define AREA_SCALE_BY (32.0lf)

/** Scales int by scale factor */
#define AREA_SCALE_INT(I) ((double)((INT) I)>>5)) 

#define AREA_WALL   1
#define AREA_WATER  2
#define AREA_STAIR  3




/**
An area is an in game region that forms a single unity in which play
takes place. Ok, its' what you'd call a "level". Every area
consists of a single tile map that determines the visual looks of the area,
a chipmunk cpSpace that simulates the dynamics, sprite information
that determines the visibility and position of the sprites, and a reference
to the scripting engine that contains the logical state of the game.

Division of the data locations: visual and physics engine: in C.
Logic/game/character data: in scripting engine.
*/
struct Area_ {
  cpSpace * space;
};

/** Allocates an area. */
Area * area_alloc() {
  return STRUCT_ALLOC(Area);
}

/** Initializes an area. */
Area * area_init(Area * self) {
  if(!self) return NULL;
  self->space = cpSpaceNew();
  return self;
}

/** Makes a new area. */
Area * area_new() {
  return area_init(area_alloc());
}

/** Deinitializes an area and returns self. */
Area * area_done(Area * self) {
  if(!self) return self;
  if(!self->space) return self;
  cpSpaceFreeChildren(self->space);
  cpSpaceFree(self->space);
  self->space = NULL;
  return self;
}

/** Frees an area. Returns NULL. */
Area * area_free(Area * self) {
  if(area_done(self)) {
    mem_free(self);
  }
  return NULL;
}

int area_addwall(Area * self, int x, int y, int z, int w, int h) {
  cpShape * shape = NULL;
}

