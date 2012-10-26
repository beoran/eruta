#include "eruta.h"

#include "area.h"
#include "mem.h"
#include "flags.h"

/** Scale factor. */
#define AREA_SCALE_BY (32.0lf)

/** Scales int by scale factor */
#define AREA_SCALE_INT(I) ((double)((INT) I)>>5)) 

/* Thing types. Generally speaking, there are 
things that can move and those that cannot. 
Chipmunks treats them differently in the sense that
static things will all use a single (or one of 
a few) static bodies, which does not need to be 
freed when the thing isn't needed anymore.
*/
#define THING_UNUSED -1
#define THING_WALL   1
#define THING_WATER  2
#define THING_STAIR  3
#define THING_ACTOR  4
#define THING_MOBILE 5
#define THING_ZONE   6

enum Thingflags_ {
  THING_FLAGS_OWNBODY  = 1,
  THING_FLAGS_OWNSHAPE = 2,
};


/**
* A Thing is any in-game object that appears the world/map view.
*/
struct Thing_ {
  int kind;  /* What kind of thing it is. Same as collision type. */
  int id;    /* Numercial ID. */
  int flags; /* State flags.  */
  Area      * area; /* Aera the thing is in if any. */
  cpBody    * body; /* Physical body of the thing. Is NULL for statical body. */
  cpShape   * shape; /* Main collision shape of the thing. */
  int         z; /* Layer the thing is in. */
  void *      data; /* Logical data of the thing. */
};

/** Sets the z value of the Thing. This influences which layer it
and, if set, it's Shape is in. Logically,
there are 4 layers in every area, 2 bottom ones and 2 top ones. 
Layer 0 is for the floor or walls itself, layer 1 for things that
are standing or lying on the floor, or for edges,
layer 2 is for things on the "top floor", that is, another 
floor above the current one which may be used for bridges, 
overpasess, etc. Layer 3 is then for things standing
or lying on that top layer.
*/
Thing * thing_z_(Thing * self, int z) {
  if (!self) return NULL;
  self->z = z;
  if(self->shape) {
    cpShapeSetLayers(self->shape, 1 << z);
  }  
  return self;
}

/** Sets an individual flag on the Thing. */
int thing_setflag(Thing * self, int flag) {
  return flags_set(&self->flags, flag);
}

/** Unsets an individual flag on the Thing. */
int thing_unsetflag(Thing * self, int flag) {
  register int wflags = self->flags;
  return flags_unset(&self->flags, flag);
}

/** Sets or unsets an individual flag on the Thing. 
If set is true the flag is set, if false it's unset. */
int thing_flag_(Thing * self, int flag, int set) {
  return flags_put(&self->flags, flag, set);
}

/** Checks if an individual flag is set. */
int thing_flag(Thing * self, int flag) {
  return flags_get(self->flags, flag);
}

/** Uninitializes a thing. */
Thing * thing_done(Thing * self) {
  if(!self) return NULL;
  if(self->body && thing_flag(self, THING_FLAGS_OWNBODY)) {
    cpBodyFree(self->body); 
  }
  if(self->shape && thing_flag(self, THING_FLAGS_OWNSHAPE)) {
    cpShapeFree(self->shape);
  }
  self->body    = NULL;
  self->shape   = NULL;
  self->area    = NULL;
  self->z       = -1;
  self->kind    = THING_UNUSED;
  return self;
}

/** Frees a thing. */
Thing * thing_free(Thing * self) {
  mem_free(thing_done(self));
  return NULL;
}


/** Allocates a Thing. */
Thing * thing_alloc() {
  return STRUCT_ALLOC(Thing);
}

/** Initializes a Thing. */
Thing * thing_init(Thing * self, int id, int kind, int z,
                   Area * area, cpBody * body, cpShape * shape) {
  if(!self) return NULL;
  self->kind    = kind;
  self->id      = id;
  self->area    = area;
  self->body    = body;
  self->shape   = shape;
  thing_z_(self, z);
  /* If the thing has a body and it is not static, 
  assume it is owned by this thing and must be freed when calling 
  thing_done.
  */
  if (self->body && !cpBodyIsStatic(self->body)) {
    thing_setflag(self, THING_FLAGS_OWNBODY);
  }
  /* Assume the shape is owned. */
  if (self->shape) {
    thing_setflag(self, THING_FLAGS_OWNSHAPE);
  }
  return self;
}

/** Initializes a Thing, and makes a new body and rectangular
shape for it. */
Thing * thing_initmake(Thing * self, int kind, int id, int z,
                       Area * area, int x, int y, int w, int h,
                       cpFloat mass, cpFloat impulse) { 
    
    cpBody * body     ; 
    cpVect points[4]  = { cpv(0, 0), cpv(0, h), cpv(w, h), cpv(w, 0)};
    cpVect offset     = cpv(x,y); 
    cpShape * shape   ; 
    
    if(!self) return NULL;
 
    body  = cpBodyNew(mass, impulse);
    if(!body) goto out_of_memory;
    shape = cpPolyShapeNew(body, 4, points, offset);
    if(!shape) goto out_of_memory;
    
    /* Shapes's friction (u) should be set to 0 so only pushing is possible, 
       not "rubbing" */
    cpShapeSetFriction(shape, 0.0);
    cpShapeSetUserData(shape, self);
    return thing_init(self, id, kind, z, area, body, shape);
    
    out_of_memory:
    thing_done(self); 
    return NULL; 
}

/** Allocates and initializes a new Thing. */
Thing * thing_new(int id, int kind, int z,
                   Area * area, cpBody * body, cpShape * shape) {
  Thing * self = thing_alloc();
  if(!thing_init(self, id, kind, z, area, body, shape)) {
    return thing_free(self); 
  }
  return self;
}

/*
An area is an in game region that forms a single unity in which play
takes place. OK, it's what you'd call a "level". Every area
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
  return -1;
}

