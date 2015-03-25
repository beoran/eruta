#include "eruta.h"

#include "area.h"
#include "tilemap.h"
#include "bump.h"
#include "thing.h"
#include "mem.h"
#include "flags.h"
#include "draw.h"
#include "sprite.h"
#include "collide.h"

#include "thing_struct.h"

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
  BumpWorld   * world;
  int           lastid;
  Thing       * things[AREA_THINGS];
  Thing       * things_todraw[AREA_THINGS];
  BumpTilemap * bumpmap;
  int           draw_physics;
};


/** Gets the amount of possible things for this area */
int area_maxthings(Area * area) {
  (void) area;
  return AREA_THINGS;
}

/** Gets the thing for the given thing ID. */
Thing * area_thing(Area * area, int index) {
  if (index < 0) return NULL;
  if (index > AREA_THINGS) return NULL;
  if (!area) return NULL;
  return area->things[index];
}

/** Sets the thing for the given thing ID. */
Thing * area_thing_(Area * area, int index, Thing * set) {
  if (index < 0) return NULL;
  if (index > AREA_THINGS) return NULL;
  if (!area) return NULL;
  return area->things[index] = set;
}


/** Returns the first unused Thing ID greater than minimum Internally walks over
 * the known things and finds an empty cell. Returns negative on error. */
int area_get_unused_thing_id(Area * self, int minimum) {
  int index, stop;
  if (!self) return -1;
  if (minimum < 0) return -2;
  stop = area_maxthings(self);
  for (index = minimum; index < stop; index++) {
    Thing * thing = area_thing(self, index);
    if(!thing) {
      return index;
    }
  }
  return -3;
}

/** Puts a thing inside the area. Returns NULL on error.
returns the thing set if ok. Sets the thing's id to it's
position in the internal array for things the area has. */
Thing * area_addthing(Area * area, int index, Thing * thing) {
  if (!thing)  return NULL;
  if (index < 0 ) return NULL;
  if (index > AREA_THINGS) return NULL;
  if(!area_thing_(area, index, thing)) return NULL;
  thing_id_(thing, index);
  /* Set last used id to index if needed */
  if (area->lastid < index)  { area->lastid = index; }
  /* Don't forget to add physical body and hull to the space if needed. */
  bumpworld_add_body(area->world, thing->physical);
  bumpworld_add_hull(area->world, thing->hull);
  return thing;
}

/** Set the tile map to use for this area. */
Tilemap *
area_addmap(Area * area, Tilemap * map) {
  return bumpworld_tilemap_(area->world, map);
}

/** Returns the static body that this area uses for static things.
 * Removed, since not needed because bump uses real tile maps and not static bodies.
 */


/* Removes the thing from the area, but does not deallocate it.
 * Returns thing if sucessful. */
Thing * area_removething(Area * area, Thing * thing) {
  if (!thing)                           return NULL;
  if (thing->id<0)                      return NULL;
  if (thing->kind == THING_UNUSED)      return NULL;
  bumpworld_remove_hull(area->world, thing->hull);
  bumpworld_remove_body_only(area->world, thing->physical);
  bumpworld_delete_hulls_for(area->world, thing->physical);
  area_thing_(area, thing->id, NULL);
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
  if(!self->world) return self;
  area_cleanupthings(self);
  bumpworld_free(self->world);
  self->world = NULL;
  self->draw_physics = FALSE;
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
  self->world   = bumpworld_new();
  if (!self->world) goto out_of_memory;
  area_emptythings(self);
  self->bumpmap = NULL;
  self->draw_physics = FALSE;
  // area_setup_default_collision_handlers(self, self);
  return self;

  out_of_memory:
  area_done(self);
  return NULL;
}

/** Sets the area so it will draw its physics
 *  bounds boxes when it is drawn or not.
 */
void area_draw_physics_(Area * self, int draw) {
  self->draw_physics = draw;
}

/** Checks if the area draws its physics or not. */
int area_draw_physics(Area * self) {
  return self->draw_physics;
}

/** Makes a new area. */
Area * area_new() {
  return area_init(area_alloc());
}


/** Returns the BumpWorld that the area uses for dynamics modelling. */
BumpWorld * area_world(Area * self) {
  if (!self) return NULL;
  return self->world;
}


/** Makes a new dynamic thing and adds it to the area. Return it or NULL on error. */
Thing * area_newdynamic(Area * self, int index, int kind,
                        int x, int y, int z, int w, int h) {
  Thing * thing;
  thing = thing_newdynamic(self, kind, x, y, z, w, h);
  if (!area_addthing(self, index, thing)) {
    return thing_free(thing);
  }
  return thing;
}


/** Sets the current tile map of the area. */
ERES area_tilemap_(Area * self, Tilemap * map) {
  int wide, high;
  if (map) {
    wide = tilemap_gridwide(map);
    high = tilemap_gridhigh(map);
  } else {
    /* No map so this is whatever, I think. */
    wide = 640;
    high = 480;
  }
  bumpworld_tilemap_(self->world, map);
  return ERES_OK;
}



/** Draws all things in an area taking the camera into account. */
void area_draw(Area * self, Camera * camera) {
  int index;
  for(index = 0; index <  (self->lastid + 1); index++) {
    Thing * thing = self->things_todraw[index];
    /* If we encounter a NULL, we're all done drawing. */
    if(!thing) break;
    thing_draw(thing, camera);
  }
#ifdef ERUTA_DEBUG_DRAW
  bumpworld_draw_debug(self->world);
#endif
}

/** Draws all things in an area taking the camera and layer into account. */
void area_draw_layer (Area * self, Camera * camera, int layer) {
  int index;
  for (index = 0; index <  (self->lastid + 1); index++) {
    Thing * thing = self->things_todraw[index];
    /* If we encounter a NULL, we're all done drawing. */
    if(!thing) break;
    /* Only draw current layer. */
    if(thing_z(thing) != layer) {
      continue;
    }
    thing_draw(thing, camera);
  }
  if (self->draw_physics) { 
    bumpworld_draw_debug(self->world);
  }
}


/** Updates the area */
void area_update(Area * self, double dt) {
  int subindex = 0;
  int index;
  bumpworld_update(self->world, dt);
  for(index = 0; index <  (self->lastid + 1); index++) {
    Thing * thing = area_thing(self, index);
    /* this is a bandaid, somehow uninitialized things are stored in the area. */
    if (thing &&  thing->physical) {
      thing_update(thing, dt);
      self->things_todraw[subindex] = thing;
      subindex++;
    }
  }
  /** Sort drawing array so the painter's algorihm can be used. */
  qsort(self->things_todraw, subindex, sizeof(Thing*), thing_compare_for_drawing);
}


/** Returns the thing related to the hull's body, if any. */
Thing * bumphull_thing(BumpHull * hull) {
  BumpBody * body = bumphull_body(hull);
  if(!body) return NULL;
  return bumpbody_data(body);
}

/* Helper for callback wrapper. */
struct area_find_hulls_helper {
  int (* callback)(Thing * thing, void * extra);
  void * extra;
};

/* Callback wrapper. */
static int area_find_hulls_callback(BumpHull * hull, void * extra) {
  struct area_find_hulls_helper * helper = extra;
  Thing * thing = bumphull_thing(hull);
  if (thing) {
    return helper->callback(thing, helper->extra);
  }
  return 0;
}


/** Finds all things in a given rectangle and calls the callback for each of them.
 * Returns 0. If the callback returns nonzero returns this in stead immediately. */
int area_find_things(Area * self, int x, int y, int w, int h, void * extra,
  int callback(Thing * thing, void * extra)) {
  struct area_find_hulls_helper helper;
  BumpAABB bounds = bumpaabb_make_int(x, y, w, h);
  helper.callback = callback;
  helper.extra    = extra;
  return bumpworld_find_hulls(self->world, bounds, &helper, area_find_hulls_callback);
}

/* Avoid allocation in the function below. */
static BumpHull * area_hulls[AREA_THINGS]; 

/** Finds all things in a given rectangle and returns
 * up to max_things of them in the array things.
 * Returns the amount of things found or negative on error. */
int area_find_and_fetch_things(Area * self, int x, int w, int y, int h,
  Thing ** things, int max_things) {
  int index, result;
  BumpAABB bounds = bumpaabb_make_int(x, y, w, h);
  int amount  = bumpworld_find_and_fetch_hulls(self->world, bounds, area_hulls, AREA_THINGS);
  int stop    = amount;
  if (stop < max_things) stop = max_things;
  result = 0;
  for (index = 0; index < stop; index ++) {
    Thing * thing = bumphull_thing(area_hulls[index]);
    if (thing) {
      things[result] = thing;
      result++;
    }
  }
  return result;
}



/** Gets the thing at index index and adds a new hull to it */
BumpHull * area_add_hull(Area * self, int index, int kind,
                      int x, int y, int z, int w, int h) {
  BumpHull * hull;
  Thing    * thing;
  BumpAABB   aabb = bumpaabb_make_int(x, y, w, h);
  thing = area_thing(self, index);
  return bumpworld_new_hull(self->world, thing->physical, bevec(x,y), aabb, z, kind);
}

/* Removes a hull from the area and also from the body it was attached 
 * to. */
Area * area_delete_hull(Area * self, int index) {
  if (bumpworld_delete_hull_index(self->world, index)) return self;
  return NULL;
}

/* Removes a body from the area and also all hulls attached to it from the area. */
Area * area_delete_body(Area * self, int index) {
  if(bumpworld_delete_body_index(self->world, index)) return self;
  return NULL;
}


/** Sets a flag on the main hull of the Thing indicated by index. */
int area_set_thing_hull_flag(Area * me, int index,  int flag) {
  Thing * thing;
  if (!me) return 0;
  thing = area_thing(me, index);
  return thing_set_hull_flag(thing, flag);
}

/** Unsets a flag for the main hull of the thing. */
int area_unset_thing_hull_flag(Area * me, int index,  int flag) {
  Thing * thing;
  if (!me) return 0;
  thing = area_thing(me, index);
  return thing_unset_hull_flag(thing, flag);
}

/** Sets all flags for the main hull of the thing. */
int area_thing_hull_flags_(Area * me, int index,  int flags) {
  Thing * thing;
  if (!me) return 0;
  thing = area_thing(me, index);
  return thing_hull_flags_(thing, flags);
}

/** Gets all flags for the main hull of the thing. */
int area_thing_hull_flags(Area * me, int index) {
  Thing * thing;
  if (!me) return 0;
  thing = area_thing(me, index);
  return thing_hull_flags(thing);
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

