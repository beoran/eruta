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

#define THING_WALL_MASS   INFINITY
#define THING_ACTOR_MASS  1.0

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

/** Gets the ID of the thing. Returns negative on error or for a
thing that doesn't belong to an area yet.*/
int thing_id(Thing * thing) {
  if(!thing) return -3;
  return thing->id;
}

/** Sets the ID of the thing. Returns negative on error or 
for a thing that doesn't belong to an area yet (as inicated by newid) 
*/
int thing_id_(Thing * thing, int newid) {
  if(!thing) return -3;
  return thing->id = newid;
}


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

/** Generic initialization of a thing Initializes a Thing. 
Sets the given values and some flags. Links the Shape given
to the Thing if shape is not NULL. 
Also calls area_addthing on the given area if it is 
not null. Returns null if that failed, but does no cleanup.  */
Thing * thing_initgeneric(Thing * self, Area * area, int kind, int z,
                   cpBody * body, cpShape * shape) {
  if(!self) return NULL;
  self->kind    = kind;
  self->id      = -1;
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
    /* Shapes's friction (u) should be set to 0 so only pushing is possible, 
       not "rubbing" */
    cpShapeSetFriction(self->shape, 0.0);
    cpShapeSetUserData(self->shape, self);
  }
  if (self->area) {
    if(!area_addthing(area, self)) return NULL;
  }
  return self;
}

/** Initializes a rectangular, static, non-rotating Thing, and makes 
a new body  and rectangular shape for it. Returns NULL on error. 
Uses the area's static body. */
Thing * thing_initstatic(Thing * self, Area * area, 
                       int kind, 
                       int x, int y, int z, int w, int h) { 
    cpBody * body     = area_staticbody(area); 
    cpVect points[4]  = { cpv(0, 0), cpv(0, h), cpv(w, h), cpv(w, 0)};
    cpVect offset     = cpv(x,y); 
    cpShape * shape   ; 
    if(!self) return NULL;
    if(!area) return NULL;
    shape = cpPolyShapeNew(body, 4, points, offset);
    if(!shape) goto out_of_memory;
    return thing_initgeneric(self, area, id, kind, z, body, shape);
    out_of_memory:
    thing_done(self); 
    return NULL; 
}

#ifdef COMMENT_
/** Initializes a thing that cannot rotate.  */
Thing * thing_initnorotate(Thing * thing, Area * area, int kind,
                           int x, int y, int z, int w, int h,
                           cpFloat mass) {
  cpFloat imp = INFINITY;
  return thing_initmake(thing, kind, id, x, y, z, w, h, mass, imp) 
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
#endif 

/* Get an interface of TArray for a pointer to Thing type */
#define TEMPLATE_T      Thing*
#define TEMPLATE_NAME   ThingArray
#define TEMPLATE_PREFIX thingarray_
#define TEMPLATE_ZERO   NULL
#include "tarray.h"

/* Get an implementation of TArray for pointer to Thing type. */
#define TEMPLATE_IMPLEMENT
#define TEMPLATE_T      Thing*
#define TEMPLATE_NAME   ThingArray
#define TEMPLATE_PREFIX thingarray_
#define TEMPLATE_ZERO   NULL
#include "tarray.h"

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
  cpSpace     * space;
  ThingArray  * things;
  int           lastid;
};

/** Returns an ID for a thing to use. Internally walks over the 
known things and finds an empty cell. Returns negative on error. */
int area_thingid(Area * self) {
  int index, stop;  
  ThingArray * things;
  if(!self) return -1;
  things = self->things;
  if(!things) return -2;
  stop = thingarray_size(things);
  for (index= 0; index < stop; index++) {
    Thing * thing = thingarray_getraw(things, index); 
    if(!thing) {
      if(index >= area->lastindex) {
        area->lastindex = index;
      ]
      return index;
    }
  }
  return -3;
}

/** Puts a thing inside the area. Returns NULL on error. 
returns the thing set if ok. Sets the thing's id to it's 
position in the internal array for things the area has. */
Thing * area_addthing(Area * area, Thing * thing) {
  int id = area_thingid(area); 
  if (id < 0 ) return NULL;
  if(!thingarray_put(area->things, id, thing)) return NULL;
  thing_id_(thing, id);
  return thing;
}

/** Returns the static body that this area uses for static things. */
cpBody * area_staticbody(Area * area) {
  if(!area) { return NULL; }
  if(!area->space) { return NULL; }
  return cpSpaceGetStaticBody(area->space);
}

/* Cleans up a thing array */
ThingArray * thingarray_cleanup(ThingArray * things) {
  int index, stop;  
  if(!things) return NULL;
  stop = thingarray_size(things);
  for (index= 0; index < stop; index++) {
    Thing * thing = thingarray_getraw(things, index); 
    if(thing) {
      thing_free(thing);
      thingarray_put(things, index, NULL); 
    }
  }
  return things;
}

/** Deinitializes an area and returns self. */
Area * area_done(Area * self) {
  if(!self) return self;
  if(!self->space) return self;
  // cpSpaceFreeChildren(self->space);
  cpSpaceFree(self->space);
  self->space = NULL;
  if(self->things) {
    thingarray_cleanup(self->things);
    thingarray_free(self->things);
    self->things = NULL;
  }
  return self;
}

/** Frees an area. Returns NULL. */
Area * area_free(Area * self) {
  if(area_done(self)) {
    mem_free(self);
  }
  return NULL;
}

/** Allocates an area. */
Area * area_alloc() {
  return STRUCT_ALLOC(Area);
}

/** Start with 1000 things. */
#define AREA_THINGS 1000

/** Initializes an area. */
Area * area_init(Area * self) {
  if(!self) return NULL;
  self->space = cpSpaceNew();
  self->things = thingarray_new(AREA_THINGS);
  if(!self->things) goto out_of_memory;
  
  
  
  return self;
  
  out_of_memory:
  area_done(self);
  return NULL;
}

/** Makes a new area. */
Area * area_new() {
  return area_init(area_alloc());
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

