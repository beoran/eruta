#include "eruta.h"

#include "area.h"
#include "mem.h"
#include "flags.h"
#include "draw.h"

/** Scale factor. */
#define AREA_SCALE_BY (32.0lf)

/** Scales int by scale factor */
#define AREA_SCALE_INT(I) ((double)((INT) I)>>5)) 


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
  int         kind;  /* What kind of thing it is. Same as collision type. */
  int         id;    /* Numercial ID. */
  int         flags; /* State flags.  */
  Area      * area; /* Area the thing is in if any. */
  cpBody    * body; /* Physical body of the thing. Is NULL for statical body. */
  cpShape   * shape; /* Main collision shape of the thing. */
  int         z; /* Layer the thing is in. */
  void *      data; /* Logical data of the thing. */
  /* Chipmunk makes it rather hard to get to the size of a 
  shape, and also since static shapes all have the same body, the position 
  of static shapes is lost. And getting the box is
  not reliable neough. So keep the size and position 
  for static shapes here even if it's slightly redundant.
  */
  Point       size; /* size of outline of shape */
  Point       spos; /* Position, merely for static shapes, for dynamic
  bodies, use cpBodyGetPos*/
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
  thing_done(self);
  mem_free(self);
  return NULL;
}


/** Allocates a Thing. */
Thing * thing_alloc() {
  return STRUCT_ALLOC(Thing);
}

/** Generates a rectangular shape for the given body
with the given position, size and offset */
cpShape * shape_rectnew(cpBody * body, 
                        cpFloat x, cpFloat y, cpFloat w, cpFloat h, 
                        cpFloat xoff, cpFloat yoff
                        ) {
    cpVect points[4]  = { cpv(x, y), cpv(x, y + h), cpv(x + w, y + h), cpv(x + w, y)};
    cpVect offset     = cpv(xoff, yoff); 
    cpShape * shape   = cpPolyShapeNew(body, 4, points, offset);
    return shape;
}

/** Returns true if the thing is static, false if not or if the Thing has no 
body. */
int thing_static_p(Thing * self) { 
  if (!self->body) return FALSE;
  return cpBodyIsStatic(self->body);
}




/** Generic initialization of a thing Initializes a Thing. 
Sets the given values and some flags. Links the Shape given
to the Thing if shape is not NULL. 
Does NOT call area_addthing on the given area. 
Returns null if that failed, but does no cleanup.  */
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
    // set this thing in the data of the body
    cpBodySetUserData(self->body, self);
  }
  /* Assume the shape is owned. */
  if (self->shape) {
    thing_setflag(self, THING_FLAGS_OWNSHAPE);
    /* Shapes's friction (u) should be set to 0 so only pushing is possible, 
       not "rubbing" */
    cpShapeSetFriction(self->shape, 0.0);
    cpShapeSetUserData(self->shape, self);
    /* */
  }
  return self;
}

/** Initializes a rectangular, static, non-rotating Thing. Uses the 
area's static body, and makes a new rectangular shape for it. 
Returns NULL on error. Uses the area's static body. */
Thing * thing_initstatic(Thing * self, Area * area, 
                       int kind, 
                       int x, int y, int z, int w, int h) { 
    cpBody * body     = area_staticbody(area); 
    cpShape * shape   ; 
    if(!self) return NULL;
    if(!area) return NULL;
    // static things are not positioned but simply generated at an offset
    shape       = shape_rectnew(body, 0, 0, w, h, x, y); 
    if(!shape) goto out_of_memory;
    // set spos and size;
    self->spos   = cpv(x, y);
    self->size   = cpv(w, h);
    
    return thing_initgeneric(self, area, kind, z, body, shape);
    out_of_memory:
    thing_done(self); 
    return NULL; 
}

/** Initializes a rectangular, dynamic, non-rotating Thing, with mass 1 
and makes a new body  and rectangular shape for it. Returns NULL on error. 
Uses the area's static body. */
Thing * thing_initdynamic(Thing * self, Area * area, 
                       int kind, int x, int y, int z, int w, int h) { 
    cpBody  * body    = NULL; 
    cpShape * shape   = NULL; 
    if(!self) return NULL;
    if(!area) return NULL;
    body              = cpBodyNew(THING_ACTOR_MASS, INFINITY); 
    if(!body) goto out_of_memory;
    // dynamic things ARE positioned correctly and do not use an offset
    // the object's shape is locally around the body
    shape             = shape_rectnew(body, 0, 0, w, h, 0, 0);
    if(!shape) goto out_of_memory;
    // set spos and size;
    self->spos   = cpv(x, y);
    self->size   = cpv(w, h);
    // set position of body
    cpBodySetPos(body, self->spos);
    
    return thing_initgeneric(self, area, kind, z, body, shape);
    out_of_memory:
    cpBodyFree(body);
    thing_done(self); 
    return NULL; 
}



/** Allocates and initializes a new static Thing. */
Thing * thing_newstatic(Area * area, 
                       int kind, 
                       int x, int y, int z, int w, int h) {
  Thing * self = thing_alloc();
  if(!thing_initstatic(self, area, kind, x, y, z, w, h)) {
    return thing_free(self); 
  }
  return self;
}

/** Allocates and initializes a new dynamic Thing. */
Thing * thing_newdynamic(Area * area, 
                       int kind, 
                       int x, int y, int z, int w, int h) {
  Thing * self = thing_alloc();
  if(!thing_initdynamic(self, area, kind, x, y, z, w, h)) {
    return thing_free(self); 
  }
  return self;
}

/** Accessors for thing's properties, forwarded to the body 
of the thing. **/


/** Position of the thing. */
Point thing_p(Thing * self) {
  return cpBodyGetPos(self->body);
}

/** Position X coordinates. */
int thing_x(Thing * self) {
  return cpBodyGetPos(self->body).x;
}

/** Position Y coordinates. */
int thing_y(Thing * self) {
  return cpBodyGetPos(self->body).y;
}

/** Width  of thing. */
int thing_w(Thing * self) {
  return self->size.x;
}

/** Height of thing. */
int thing_h(Thing * self) {
  return self->size.y;
}



/** Center of thing on x axis. */
int thing_cx(Thing * self) {
  return thing_x(self) + thing_w(self) / 2;
}

/** Height of thing. */
int thing_cy(Thing * self) {
  return thing_y(self) + thing_h(self) / 2;
}

/** Layer of thing. */
int thing_z(Thing * self) {
  return self->z;
}

/** Velocity of the thing. */
Point thing_v(Thing * self) {
  return cpBodyGetVel(self->body);
}

/** Speed X coordinates. */
int thing_vx(Thing * self) {
  return cpBodyGetVel(self->body).x;
}

/** Speed Y coordinates. */
int thing_vy(Thing * self) {
  return cpBodyGetVel(self->body).y;
}

/** Set velocity. */
void thing_v_(Thing * self, Point v) {
  cpBodySetVel(self->body, v);
}

/** Set velocity by xy. */
void thing_vxy_(Thing * self, int vx, int vy) {
  Point v = cpv(vx, vy);
  cpBodySetVel(self->body, v);
}

/** Set x velocity only, leaving y unchanged . */
void thing_vx_(Thing * self, int vx) {
  Point v = thing_v(self);
  v.x     = vx;
  thing_v_(self, v);
}

/** Set y velocity only, leaving x unchanged . */
void thing_vy_(Thing * self, int vy) {
  Point v = thing_v(self);
  v.y     = vy;
  thing_v_(self, v);
}

/** Sets position of thing's body. */
void thing_p_(Thing * self, Point p) {
  cpBodySetPos(self->body, p);
}

/** Adds delta to the position of thing's body. */
void thing_deltap(Thing * self, Point delta) {
  Point old = thing_p(self);
  thing_p_(self, cpvadd(old, delta));
  // cpBodySetPos(self->body, p);
}

/** Set position by xy. */
void thing_pxy_(Thing * self, int x, int y) {
  Point p = cpv(x, y);
  cpBodySetPos(self->body, p);
}

/** Set x velocity only, leaving y unchanged . */
void thing_x_(Thing * self, int x) {
  Point p = thing_p(self);
  p.x     = x;
  thing_p_(self, p);
}

/** Set x velocity only, leaving y unchanged . */
void thing_y_(Thing * self, int y) {
  Point p = thing_p(self); 
  p.y     = y;
  thing_p_(self, p);
}

/** Applies a force on the center of gravity of the thing. */
void thing_applyforce(Thing * thing, const Point f) {
  cpBodyApplyForce(thing->body, f, cpvzero);
}

/** Applies an impulse on the center of gravity of the thing. */
void thing_applyimpulse(Thing * thing, const Point f) {
  cpBodyApplyImpulse(thing->body, f, cpvzero);
}

/** Resets the force on this thing to 0. */
void thing_resetforces(Thing * thing) {
  cpBodyResetForces(thing->body);
}


/** Draws a thing to the current active drawing target, corresponding 
to it's shape and kind and taking the camera into account.  Useful for 
checking or debugging the physics. */
void thing_draw(Thing * self, Camera * camera) {
  int cx      ; 
  int cy      ;
  int drawx, x;
  int drawy, y;
  int w, h    ;
  int t       = 2;
  Color color;
  // don't draw null things.
  if(!self) return;
  cx          = camera_at_x(camera);
  cy          = camera_at_y(camera);
  w           = self->size.x;
  h           = self->size.y;
  if (thing_static_p(self)) {
    x         = self->spos.x;
    y         = self->spos.y;
    color     = color_rgbaf(1.0, 1.0, 0.0, 0.001);
  } else {
    color     = color_rgb(128, 255, 255);
    Point pos = thing_p(self);
    x         = pos.x;
    y         = pos.y;
    t         = 8;
  }
  /* Do not draw out of camera range. */
  if(!camera_cansee(camera, x, y, w, h)) {
    return;
  }
  drawx = x - cx;
  drawy = y - cy;
  draw_box(drawx, drawy, w, h, color, t);
}



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
An area is an in game region that corrresponds with a single loaded tile map
and that keeps track of the physics and location of all in-game objects. 

An area consists of a chipmunk cpSpace that simulates the dynamics, 
and a list of Things, that is, in game objects. It also contains a reference
to a SpriteList (that is owned by state). 

Division of the data locations: visual and physics engine: in C.
Logic/game/character data: in scripting engine.
*/

/* Start with space for 20000 things. */
#define AREA_THINGS 20000

struct Area_ {
  cpSpace     * space;
  int           lastid;
  Thing       * things[AREA_THINGS];
};

/** Gest the amount of possible things for this area */
int area_maxthings(Area * area) {
  return AREA_THINGS;
}

/** Gets the thing for the given thing ID. */
Thing * area_thing(Area * area, int index) {
  if(index < 0) return NULL;
  if(index > AREA_THINGS) return NULL;
  if(!area) return NULL;
  return area->things[index];
} 

/** Sets the thing for the given thing ID. */
Thing * area_thing_(Area * area, int index, Thing * set) {
  if(index < 0) return NULL;
  if(index > AREA_THINGS) return NULL;
  if(!area) return NULL;
  return area->things[index] = set;
} 


/** Returns an ID for a thing to use. Internally walks over the 
known things and finds an empty cell. Returns negative on error. */
int area_thingid(Area * self) {
  int index, stop;
  if(!self) return -1;
  stop = area_maxthings(self);
  for (index= 0; index < stop; index++) {
    Thing * thing = area_thing(self, index); 
    if(!thing) {
      if(index >= self->lastid) {
        self->lastid = index;
      } 
      return index;
    }
  }
  return -3;
}

/** Puts a thing inside the area. Returns NULL on error. 
returns the thing set if ok. Sets the thing's id to it's 
position in the internal array for things the area has. */
Thing * area_addthing(Area * area, Thing * thing) {
  int id; 
  if (!thing)  return NULL;
  id = area_thingid(area);
  if (id < 0 ) return NULL;
  if(!area_thing_(area, id, thing)) return NULL;
  thing_id_(thing, id);
  /* Don't forget to add body to shape. */
  cpSpaceAddShape(area->space, thing->shape);
  if(!thing_static_p(thing)) {
    cpSpaceAddBody(area->space, thing->body); 
  }
  return thing;
}

/** Returns the static body that this area uses for static things. */
cpBody * area_staticbody(Area * area) {
  if(!area) { return NULL; }
  if(!area->space) { return NULL; }
  return cpSpaceGetStaticBody(area->space);
}


/* Removes a thing from the area, selected by ID and deallocates it. */
int area_deletething(Area * area, int index) {
  Thing * thing = area_thing(area, index); 
  if(thing && thing->kind != THING_UNUSED) {
     thing_free(thing);
     area_thing_(area, index, NULL);
  }
  return 0;
} 


/* Cleans up the things of the area */
Area * area_cleanupthings(Area * self) {
  int index, stop;
  if(!self) return NULL;
  stop = area_maxthings(self);
  for (index= 0; index < stop; index++) {
    area_deletething(self, index);
  }
  return self;
}

/* Empties the things of an area. Does not destroy them! */
Area * area_emptythings(Area * self) {
  int index, stop;  
  if (!self) return NULL;
  stop = area_maxthings(self);
  for (index= 0; index < stop; index++) {
    area_thing_(self, index, NULL);
  }
  return self;
}

/** Deinitializes an area and returns self. */
Area * area_done(Area * self) {
  if(!self) return self;
  if(!self->space) return self;
  // cpSpaceFreeChildren(self->space);
  cpSpaceFree(self->space);
  self->space = NULL;
  if(self->things) {
    area_cleanupthings(self);
    // self->things = NULL;
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

/** Initializes an area. */
Area * area_init(Area * self) {
  if(!self) return NULL;
  self->lastid  = 0;
  self->space   = cpSpaceNew();
  area_emptythings(self);


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


/** Makes a new static thing and adds it to the area. Return it or NULL on error. */
Thing * area_newstatic(Area * self, int kind, 
                        int x, int y, int z, int w, int h) {
  Thing * thing;
  thing = thing_newstatic(self, kind, x, y, w, w, h);
  if (!area_addthing(self, thing)) { 
    return thing_free(thing);
  }
  return thing;
}


/** Makes a new dynamic thing and adds it to the area. Return it or NULL on error. */
Thing * area_newdynamic(Area * self, int kind, 
                        int x, int y, int z, int w, int h) {
  Thing * thing;
  thing = thing_newdynamic(self, kind, x, y, w, w, h);
  if (!area_addthing(self, thing)) { 
    return thing_free(thing);
  }
  return thing;
}



/** Draws all things in an area taking the camera into account. */
void area_draw(Area * self, Camera * camera) {
  int index;
  //printf("Rendering %d things\n",  self->lastid);
  for(index = 0; index <  (self->lastid + 1); index++) {
    Thing * thing = area_thing(self, index);
    if (thing) thing_draw(thing, camera);
  }
}

/** Updates the area */
void area_update(Area * self, double dt) {
  cpSpaceStep(self->space, dt);  
}


#define THING_TRACK_DELTA 32.0 * 4 

#ifdef COMMENT_

/** A tracker function for tracking a Thing. Only works with dynamic things. */
int thing_track(Tracker * tracker, void * data) {
  Thing * thing = NULL;
  if(!tracker || !tracker->camera) return TRACKER_ERROR;
  thing         = (Thing *) tracker->target;
  if(thing_static_p(thing)) return TRACKER_ERROR;
  // TODO: correct with half width and half height
  camera_centerdelta_(tracker->camera, thing_p(thing), THING_TRACK_DELTA);  
  return TRACKER_DONE;
}


#endif

