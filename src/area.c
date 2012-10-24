#include "eruta.h"

#include "area.h"
#include "mem.h"

/** Scale factor. */
#define AREA_SCALE_BY (32.0lf)

/** Scales int by scale factor */
#define AREA_SCALE_INT(I) ((double)((INT) I)>>5)) 

#define THING_WALL   1
#define THING_WATER  2
#define THING_STAIR  3
#define THING_ACTOR  4
#define THING_MOBILE 5
#define THING_ZONE   6

/**
* A Thing is any in-game object that appears the world/map view
*/
struct Thing_ {
  int kind; /* What kind of thing it is. Same as collision type. */
  int id;   /* Numercial ID. */
  Area      * area; /* Aera the thing is in if any. */
  cpBody    * body; /* Physical body of the thing. Is NULL for statical body. */
  cpShape   * shape; /* Main collision shape of the thing. */
  int         z; /* Layer the thing is in. */
};

/** Initializes a Thing. */
Thing * thing_init(Thing * self, int kind, int id, int z,
                   Area * area, cpBody * body, cpShape * shape) {
  if(!self) return NULL;
  self->kind    = kind;
  self->id      = id;
  self->z       = z;
  self->area    = area;
  self->body    = body;
  self->shape   = shape;
  return self;
}

/** Initializes a Thing, and makes a new body and rectangulat shape for it. */
Thing * thing_initmake(Thing * self, int kind, int id, int z,
                       Area * area, int x, int y, int w, int h,
                       cpFloat mass, cpFloat impulse) { 
    
    cpBody * body     = cpBodyNew(mass, impulse);
    cpVect points[4]  = { cpv(0, 0), cpv(0, h), cpv(w, h), cpv(w, 0)};
    cpVect offset     = cpv(x,y); 
    cpShape * shape   = cpPolyShapeNew(body, 4, points, offset);
    /* Shapes's friction (u) should be set to 0 so only pushing is possible, 
       not "rubbing" */
    if(!self) return NULL;
    cpShapeSetFriction(shape, 0.0);
    cpShapeSetUserData(shape, self);
    return thing_init(self, kind, id, z, area, body, shape);
}

/*
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
  // cpSpaceFreeChildren(self->space);
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

/** Returns the cpSpace that the area uses for dynamics modelling. */
cpSpace * area_space(Area * self) {
  if (!self) return NULL;
  return self->space;
}

int area_addwall(Area * self, int x, int y, int z, int w, int h) {
  cpShape * shape = NULL;
}

