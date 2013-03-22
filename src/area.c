#include "eruta.h"

#include "area.h"
#include "thing.h"
#include "mem.h"
#include "flags.h"
#include "draw.h"
#include "sprite.h"

/** Scale factor. */
#define AREA_SCALE_BY (32.0lf)

/** Scales int by scale factor */
#define AREA_SCALE_INT(I) ((double)((INT) I)>>5)) 


/*
An area is an in game region that corresponds with a single loaded tile map
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
  Thing       * things_todraw[AREA_THINGS];
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


/* Removes the thing from the area, but does not deallocate it. 
 * Returns thing if sucessful. */
Thing * area_removething(Area * area, Thing * thing) {
  if (!thing)                           return NULL;
  if (thing->id<0)                      return NULL;
  if (thing->kind == THING_UNUSED)      return NULL;
  area_thing_(area, thing->id, NULL);
  if (cpSpaceContainsShape(area->space, thing->shape)) { 
    cpSpaceRemoveShape(area->space, thing->shape);
  }
  if (cpSpaceContainsBody(area->space, thing->body)) { 
    if(!thing_static_p(thing) && (thing->body != area_staticbody(area)) ) {
      cpSpaceRemoveBody(area->space, thing->body); 
    }
  }
  return thing;
}


/* Removes a thing from the area, selected by ID and deallocates it. */
int area_deletething(Area * area, int index) {
  Thing * thing = area_thing(area, index); 
  Thing * del   = area_removething(area, thing);
  if(thing && del) {
     thing_free(thing);
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
  area_cleanupthings(self);
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
    Thing * thing = self->things_todraw[index];
    if(!thing) break;
    /*
    area_thing(self, index);
    if (thing) */
    thing_draw(thing, camera);
  }
}

/** Updates the area */
void area_update(Area * self, double dt) {
  int subindex = 0;
  int index;
  cpSpaceStep(self->space, dt);  
  for(index = 0; index <  (self->lastid + 1); index++) {
    Thing * thing = area_thing(self, index);
    if (thing) { 
      thing_update(thing, dt);
      self->things_todraw[subindex] = thing;
      subindex++;
    }
  }
  /** Sort drawing array for drawing. */
  qsort(self->things_todraw, subindex, sizeof(Thing*), thing_compare_for_drawing); 
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

