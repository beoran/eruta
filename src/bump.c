#include <math.h>

#include "eruta.h"
#include "camera.h"
#include "callrb.h"
#include "bump.h"
#include "bumpshape.h"

#include "mem.h"
#include "dynar.h"

#include "state.h"
#include "draw.h"
#include "monolog.h"

/**
 * Some design ideas:
 * 
 * ATTACKS: attacks are modelled as hulls that are attached 
 * to the world and body of the attacking thing and set to a 
 * kind of BUMP_KIND_SENSOR | BUMP_KIND_ATTACK. 
 * 
 * Such hulls do collide but do not cause a physical response, 
 * only the script is notified of the attack hitting home. 
 * Maybe some flags need to be set or would be helpful to be set as well?
 * 
 * Same idea could be used to improve the current search, with a kind of
 * BUMP_KIND_SENSOR | BUMP_KIND_SEARCH. 
 * 
 * To avoid too many memory allocations, it might be a good idea to 
 * let each Thing allocate a few hulls for attack and search, and then reuse 
 * them.  That would require an additional BUMP_FLAG_DISABLED flag.
 * 
 * Problem with this idea: what about mobile atacs and projectiles, like arrows, 
 * gun shots or rays? Perhaps attacks should be better modeled as linked Things 
 * in stead? But even in that case such a thing would still need a sensor hull.
 * 
 * Also, the advantage of using a Thing for even non-ranged attacks is that 
 * then a sprite animation can be used to "show" the attack. 
 * And since a thing can have multiple hulls, this can be used for "split"
 * attacks. So things for attacks it is!
 * 
 */



enum BumpLock_ {
  BUMP_LOCK_NORTH = 1 << 0,
  BUMP_LOCK_WEST  = 1 << 1,
  BUMP_LOCK_SOUTH = 1 << 2,
  BUMP_LOCK_EAST  = 1 << 3,
};



struct BumpBody_ {
  int id;
  BeVec p;
  BeVec v;
  BeVec a;
  BeVec p_impulse;
  BeVec v_impulse;
  BeVec a_impulse;
  BeVec p_next;
  BeVec v_next;
  BeVec a_next;
  BeVec f;
  float   m;
  int     locks;
  void   *data;
};


struct BumpHull_ {
  int         id;
  BumpBody  * body;
  BeVec       delta;
  /* Use bumphull_aabb_real, NOT bounds in most cases. */
  BumpAABB    bounds;
  int         layers;
  /* Type of hull */
  int         kind;
  int         support;
  /* Flags of the hull. */
  int         flags;
};

typedef struct BumpHash_        BumpHash;
typedef struct BumpHashElement_ BumpHashElement;
typedef struct BumpHashBucket_  BumpHashBucket;

/* Elemenst to store in a BumpHash spatial hash table. */
struct BumpHashElement_ {
  BumpHashElement * next;
  void * data;
  int    mult_x;
  int    mult_y;
  int    tile_x;
  int    tile_y;
};

/* A Hash bucket. */
struct BumpHashBucket_ {
  struct BumpHashElement_ * element;
  int size;
};

/* Spacial hashing with a twist: so the size of the table can stay constant, 
 * the virtual grid  of the hash is "repeated" in the x and Y direction. 
 * This means that very distant objects can be together in one hash cell, however,
 * it should still be fast to do collision since a multiplier is set, and if that 
 * isn't equal, the far-away object cqn be filtered out.  
 */

#define BUMPHASH_TILE_W 64
#define BUMPHASH_TILE_H 64
#define BUMPHASH_GRID_W 64
#define BUMPHASH_GRID_H 64
#define BUMPHASH_BUCKETS        ((BUMPHASH_GRID_W)*(BUMPHASH_GRID_H))
#define BUMPHASH_W              ((BUMPHASH_GRID_W)*(BUMPHASH_TILE_W))
#define BUMPHASH_H              ((BUMPHASH_GRID_H)*(BUMPHASH_TILE_H))

#define BUMP_WORLD_MAXBODIES 1000
#define BUMP_WORLD_MAXHULLS  5000
#define BUMPHASH_ELEMENTS    5000


struct BumpHash_ {  
  /* Elements. */
  BumpHashElement elements[BUMPHASH_ELEMENTS];
  int last_element;
  /* Buckets. */
  BumpHashBucket buckets[BUMPHASH_BUCKETS];
};



BumpHash * bumphash_empty(BumpHash * self) {
  int index;
  if (!self) return NULL;
  self->last_element = 0;
  for(index = 0; index < BUMPHASH_BUCKETS; index++) {
      BumpHashBucket * bucket = self->buckets + index;
      bucket->element = NULL;
      bucket->size    = 0;
  }
  for(index = 0; index < BUMPHASH_ELEMENTS; index++) {
      BumpHashElement * element = self->elements + index;
      element->data   = NULL;
      element->next   = NULL;
      element->mult_x = -1;
      element->mult_x = -1;
  }
  return self;
};


BumpHash * bumphash_init(BumpHash * self) {
  return bumphash_empty(self);
}


BumpHashElement * 
bumphashelement_init(BumpHashElement * element, int x, int y, void * data) {  
  element->data         = data;
  element->tile_x       = x / BUMPHASH_W;
  element->tile_y       = y / BUMPHASH_H;  
  element->mult_x       = x % BUMPHASH_W;
  element->mult_y       = y % BUMPHASH_H;  
  return element;
}


int bumphash_index(BumpHash * self, int x, int y) {
  int index = x / BUMPHASH_W;
  index    += (y / BUMPHASH_H) * BUMPHASH_GRID_W;
  /* convert 2d index to 1d index. */
  (void) self;
  return index;
}

BumpHashBucket * bumphash_getbucket(BumpHash * self, int x, int y) {
  int index            = bumphash_index(self, x, y);
  return self->buckets + index;  
}

BumpHashElement * bumphash_add(BumpHash * self, int x, int y, void * data) {
  int index;
  BumpHashElement * element, *next_element;
  BumpHashBucket  * bucket;
  if (self->last_element >= BUMPHASH_ELEMENTS) {
    return NULL;
  }
  element               = self->elements + self->last_element;
  bumphashelement_init(element, x, y, data);  
  bucket                = bumphash_getbucket(self, x, y);
  next_element          = bucket->element; 
  element->next         = next_element;
  bucket->element       = element;
  return element;
}

/* First bump hash element. Use bumphashelement_next_at to get other objects in the same location. 
 * That are linked to this one. 
 */
BumpHashElement * bumphash_getelement(BumpHash * self, int x, int y) {
  BumpHashElement       * element;
  BumpHashBucket        * bucket;
  bucket                = bumphash_getbucket(self, x, y);
  return bucket->element;  
}

int bumphashelement_ok(BumpHashElement * self, int x, int y) {
  int mult_x       = x % BUMPHASH_W;
  int mult_y       = y % BUMPHASH_H;  
  return ((self->mult_x == mult_x) && (self->mult_y == mult_y));
}


struct BumpWorld_ {
  Dynar       * bodies;
  Dynar       * hulls;  
  int           body_max;
  int           hull_max;
  Tilemap     * map;
  BumpHash      hash;
};


/** Allocates a physical body. */
BumpBody * bumpbody_alloc() { 
  return STRUCT_ALLOC(BumpBody);  
}

/** Initializes a physical body. */
BumpBody * bumpbody_init(BumpBody * self, BeVec p, double mass) {
  if(!self) return NULL;
  self->a         = bevec0();
  self->a_impulse = bevec0();
  self->a_next    = bevec0();
  self->v         = bevec0();
  self->v_impulse = bevec0();
  self->v_next    = bevec0();
  self->p_impulse = bevec0();
  self->p_next    = bevec0();  
  self->f         = bevec0();
  self->p         = p;
  
  self->m         = mass;
  self->id        = -1;
  self->locks     = FALSE;
  self->data      = NULL;
  return self;  
}

/* Allocates and initializes a body. */
BumpBody * bumpbody_new(BeVec p, double mass) {
  return bumpbody_init(bumpbody_alloc(), p, mass);  
}

BumpBody * bumpbody_done(BumpBody * self) {
  if(!self) return self;
  self->m         = 0.0;
  self->id        = -1;
  return self;
}

BumpBody * bumpbody_free(BumpBody * self) {
  bumpbody_done(self);
  return mem_free(self);
}




BeVec bumpbody_p(BumpBody  * self) {   
  return self->p;
}

BeVec bumpbody_v(BumpBody  * self) {   
  return self->v;
}

BeVec bumpbody_a(BumpBody  * self) {   
  return self->a;
}

/* Gets data of body. In essence, this should be the Thing that owns the body. */
void * bumpbody_data(BumpBody  * self) {   
  return self->data;
}

/* Sets data of body. In essence, this should be the Thing that owns the body. */
void bumpbody_data_(BumpBody  * self, void * data) {   
  self->data = data;
}


BeVec bumpbody_p_(BumpBody  * self, BeVec vec) {   
  return self->p = vec;
}

BeVec bumpbody_v_(BumpBody  * self, BeVec vec) {   
  self->v = vec;
  return self->v;
}

BeVec bumpbody_a_(BumpBody  * self, BeVec vec) {   
  return self->a = vec;
}

BeVec bumpbody_p_impulse(BumpBody  * self, BeVec v) {  
  return self->p_impulse = v;
}

BeVec bumpbody_v_impulse(BumpBody  * self, BeVec v)  {  
  return self->v_impulse = v;
}

BeVec bumpbody_a_impulse(BumpBody  * self, BeVec v) {  
  return self->a_impulse = v;
}


void bumpbody_applyforce(BumpBody  * self, BeVec v) {
  if (!self) return;
  self->f = bevec_add(self->f, v);  
}

void bumpbody_applyimpulse(BumpBody  * self, BeVec v) {
  if (!self) return;
  self->p_impulse = bevec_add(self->p_impulse, v);  
}

void bumpbody_resetforces(BumpBody  * self) {
  self->f = bevec0();
}


BumpHull * bumphull_alloc() { 
  BumpHull  * hull = STRUCT_ALLOC(BumpHull);
  return hull;
}

/* Initializes a collision hull. */
BumpHull * bumphull_initall(
  BumpHull * self, BumpBody * body, BeVec delta, 
  BumpAABB bounds, int layers, int kind
) {
  if(!self) return NULL;
  self->body    = body;
  self->delta   = delta;
  self->bounds  = bounds;
  self->layers  = layers;
  self->kind    = kind;
  self->id      = -1;
  self->support = FALSE;
  self->flags   = BUMP_FLAG_NORMAL;
  return self;
  
}


/* Returns the true if the hull is "supported", that is, if there was
* any non-null tile under it last time the world was updates. 
* Return false if not.
*/
int bumphull_support(BumpHull * self) {
  return self->support;
}

/* Sets the hull's support state */
int bumphull_support_(BumpHull * self, int level) {
  return self->support = level;
}



/* Initializes a collision hull. */
BumpHull * bumphull_init(BumpHull * self, BumpBody * body, BumpAABB bounds) {
  return bumphull_initall(self, body, bevec0(), bounds, 1, -1);
}

BumpHull * bumphull_newall(
  BumpBody * body, BeVec delta, BumpAABB bounds, int layers, int kind
) {
  return bumphull_initall(bumphull_alloc(), body, delta, bounds, layers, kind);
}

BumpHull * bumphull_new(BumpBody * body, BumpAABB bounds) {
  return bumphull_init(bumphull_alloc(), body, bounds);
}

BumpHull * bumphull_done(BumpHull * self) {
  /* Do not deallocate the body, it's a weak reference only. */
  if(!self) return NULL;
  self->body = NULL;
  return self;
}
 
 
/** Sets the layers of a bumphull */
BumpHull * bumphull_layers_(BumpHull * hull, int layers) {
  if(!hull) return NULL;
  hull->layers = layers;
  return hull;
} 

/** Gets the layers of a bumphull. Returns negative on error.*/
int bumphull_layers(BumpHull * hull) {
  if (!hull) return -1;
  return hull->layers;
} 


/** Sets the kind of a bumphull. */
BumpHull * bumphull_kind_(BumpHull * hull, int kind) {
  if(!hull) return NULL;
  hull->kind = kind;
  return hull;
} 

/** Gets the kind of a bumphull. Returns negative on error.*/
int bumphull_kind(BumpHull * hull) {
  if (!hull) return -1;
  return hull->kind;
} 

/** Gets the ID or index of a bumphull. Returns negative on error.*/
int bumphull_id(BumpHull * hull) {
  if (!hull) return -1;
  return hull->id;
} 


/** Gets the bounds box of the bump hull. */
BumpAABB bumphull_aabb(BumpHull * hull) {
  return hull->bounds;
}

/** Gets a pointer to the the bounds box of the bump hull. */
BumpAABB * bumphull_aabbptr(BumpHull * hull) {
  return &(hull->bounds);
}

/** Gets the bounds box of the bump hull and moves it to the 
 * correct position relative to the hull's body (if any). 
 */
BumpAABB bumphull_aabb_real(BumpHull * hull) {
  BumpAABB bounds = hull->bounds;
  if (hull->body) { 
    bounds.p        = bevec_add(hull->body->p, hull->delta);
  } else {
    bounds.p        = hull->delta;
  }
  return bounds;
}

/** Gets the bounds box of the bump hull and moves it to the 
 * correct position relative to NEXT position of the the hull's body (if any). 
 */
BumpAABB bumphull_aabb_next(BumpHull * hull) {
  BumpAABB bounds = hull->bounds;
  if (hull->body) { 
    bounds.p        = bevec_add(hull->body->p_next, hull->delta);
  } else {
    bounds.p        = hull->delta;
  }
  return bounds;
}


/**
 * Returns a pointer the physical body of the hull if set, or NULL.
 */
BumpBody * bumphull_body(BumpHull * hull) {
  if (!hull) return NULL;
  return hull->body;
}

/** Returns a pointer to the data of the body of the hull.
 * This should be the Thing that owns that body. Returns NULL
 * if no body or body data is set. */
void * bumphull_body_data(BumpHull * hull) {
  return bumpbody_data(bumphull_body(hull));
}



/* Determines the lock flags to set for a given push back vector. */
int bevec_lock_flags(BeVec vec) {
  int lock = 0;
  if (vec.x > 0.0) lock |= BUMP_LOCK_WEST;  
  if (vec.x < 0.0) lock |= BUMP_LOCK_EAST;  
  if (vec.y > 0.0) lock |= BUMP_LOCK_NORTH;  
  if (vec.y < 0.0) lock |= BUMP_LOCK_SOUTH;  
  return lock;
}

/*  Applies the locks to vec to avoid any motion in the locked directions indicated by lock.  */
BeVec bevec_lock(BeVec vec, int lock) {
  BeVec res = vec;
  if (((lock & BUMP_LOCK_WEST)  == BUMP_LOCK_WEST)  && (vec.x < 0.0)) res.x = 0.0;
  if (((lock & BUMP_LOCK_EAST)  == BUMP_LOCK_EAST)  && (vec.x > 0.0)) res.x = 0.0;  
  if (((lock & BUMP_LOCK_NORTH) == BUMP_LOCK_NORTH) && (vec.y < 0.0)) res.y = 0.0;  
  if (((lock & BUMP_LOCK_SOUTH) == BUMP_LOCK_SOUTH) && (vec.y > 0.0)) res.y = 0.0;  
  return res; 
}





/* Calculate the collision pushback vector. */
BeVec bumpaabb_collision_pushback(BumpAABB self, BumpAABB other, BeVec motion, double dt) {
  BeVec overlap = bumpaabb_overlap_vector(self, other);
  double angle  = bevec_toangle(motion);
  BeVec dp      = bevec_sub(other.p, self.p);
  double aidx   = 0.0, aidy = 0.0;
  double fx, fy, vn;
  BeVec res     = bevec0();
  BeVec step    = bevec_mul(motion, dt);
  if (bevec_length(motion) < 0.01) {
    step        = bevec_sub(other.p, self.p);
    angle       = bevec_toangle(step);
  }
  
  /* Calculate the normal speed. Do not resolve collision if 
   * objects are already moving away from each other. The advantage of 
   * this is that objects that are  moving away and that would be pushed 
   * back by collision resolution might  jump back too far. It also seems 
   * to have some stabilizing side effects.
   */ 
  vn = bevec_dot(step, dp);
  if (vn < 0.0) {  return res; }

  
  /* Calculate overlap based push back vector. */
  fx = 1.0;
  if (step.x < 0.0) {    
    aidx = bumpaabb_overlap_x(self, other);     
  } else if (step.x > 0.0) {
    aidx = bumpaabb_overlap_x(self, other);     
    fx = -1.0;
  } else { 
    fx = -1.0;
    aidx  = bumpaabb_overlap_x(self, other);     
    if (self.p.x < other.p.x) fx = 1.0;
  }
  
  fy = 1.0;
  if (step.y < 0.0) {
    aidy = bumpaabb_overlap_y(self, other);   
  } else if (step.y > 0.0) {
    aidy = bumpaabb_overlap_y(self, other);
    fy = -1.0;
  } else { 
    aidy = bumpaabb_overlap_y(self, other);     
    if (self.p.y < other.p.y) fy = -1.0;
  }
  
  if (fabs(aidx) < fabs(aidy)) {
    res.x = aidx * fx;     
  } else if (fabs(aidx) > fabs(aidy)) { 
    res.y = aidy * fy; 
  } else {
    res.x = aidx * fx;     
  }

  return res; 
}



BumpHull * bumphull_free(BumpHull * self) {
  bumphull_done(self);
  return mem_free(self);
}


/** Returns the flags of the hull, or -1 if the hull is NULL.*/
int bumphull_flags(BumpHull * hull) {
  if (!hull) return -1;
  return hull->flags;
}

/** Sets the flags of the hull. Returns the flags set or -1 if the hull is NULL
 */
int bumphull_flags_(BumpHull * hull, int flags) {
  if (!hull) return -1;
  return hull->flags = flags;
}

/** Gets an individual flag of the hull. Returns 0 if the hull is NULL.
 * Otherwse returns ninzero if the flag is set, and zero if it isn't. */
int bumphull_get_flag(BumpHull * hull, int flag) {
  if (!hull) return 0;
  return hull->flags & flag;
}

/** Sets an individual flag of the hull. Return 0 if the hull is NULL.
 * Otherwse returns nonzero if the flag is set, and zero if it isn't. */
int bumphull_set_flag(BumpHull * hull, int flag) {
  if (!hull) return 0;
  hull->flags &= flag;  
  return bumphull_get_flag(hull, flag);
}

/** Unsets an individual flag of the hull. Return 0 if the hull is NULL.
 * Otherwse returns nonzero if the flag is set, and zero if it isn't. */
int bumphull_unset_flag(BumpHull * hull, int flag) {
  if (!hull) return 0;
  hull->flags &= (~flag);  
  return bumphull_get_flag(hull, flag);
}



BumpWorld * bumpworld_alloc() {
  return STRUCT_ALLOC(BumpWorld);
}



/* Initializes a world. */
BumpWorld * bumpworld_init(BumpWorld * self) {
  if (!self) return NULL;
  self->body_max      = 0;
  self->hull_max      = 0;
  self->bodies          = dynar_newptr(BUMP_WORLD_MAXBODIES);
  self->hulls           = dynar_newptr(BUMP_WORLD_MAXHULLS);
  self->map             = NULL;
  bumphash_init(&self->hash);
  return self;
}

/* Allocates and initializes a world. */
BumpWorld * bumpworld_new(void) {
  return bumpworld_init(bumpworld_alloc());
}

/* Cleans up the bump map after use. */
BumpWorld * bumpworld_done(BumpWorld * self) {
  if(!self) return NULL;
  self->bodies = dynar_free(self->bodies);
  self->hulls  = dynar_free(self->hulls);
  return self;
}

BumpWorld * bumpworld_free(BumpWorld * self) {
  if(self) bumpworld_done(self);
  return mem_free(self);
}

/** Fetches a body with a given index from the world. */
BumpBody * bumpworld_body(BumpWorld * self, int index) {
  return dynar_getptr(self->bodies, index);
}

/** Fetches a hull with a given index from the world. */
BumpHull * bumpworld_hull(BumpWorld * self, int index) {
  return dynar_getptr(self->hulls, index);
}

/** Returns the maximum body count for use in loops. */
int bumpworld_body_count(BumpWorld * self) {
  return dynar_size(self->bodies);
}

/** Returns the maximum hull count for use in loops. */
int bumpworld_hull_count(BumpWorld * self) {
  return dynar_size(self->hulls);
}

/** Adds a body to this world. */
BumpBody * bumpworld_add_body(BumpWorld * self, BumpBody * body) {
  int index;
  if(!self) return NULL;
  if(!body) return NULL;
  for (index = 0; index < bumpworld_body_count(self) ; index ++) {
    if (!dynar_getptr(self->bodies, index)) {
      if(!dynar_putptr(self->bodies, index, body)) return NULL;
      body->id = index;      
      if (index >= self->body_max) self->body_max = index + 1;
      return body;
    }
  }
  return NULL;
}

/** Adds a hull to this world. */
BumpHull * bumpworld_add_hull(BumpWorld * self, BumpHull * hull) {
  int index;
  if(!self) return NULL;
  if(!hull) return NULL;
  // for (index = 0; index < self
  for (index = 0; index < bumpworld_hull_count(self) ; index ++) {
    if (!dynar_getptr(self->hulls, index)) {
      if(!dynar_putptr(self->hulls, index, hull)) return NULL;
      hull->id  = index;      
      if (index >= self->hull_max) self->hull_max = index + 1;
      return hull;
    }
  }
  return NULL;
}

/** Removes the hull but does not free it. */
BumpHull * bumpworld_remove_hull(BumpWorld * self, BumpHull * hull) {
  if(!self) return NULL;
  if(!hull) return NULL;  
  if (hull->id < 0) return NULL;
  dynar_putptr(self->hulls, hull->id, NULL);
  hull->id = -1;  
  self->hull_max--;
  return hull;
}

/** Removes the hull and frees it. Returns NULL. */
BumpWorld * bumpworld_delete_hull(BumpWorld * self, BumpHull * hull) {
  if(!bumpworld_remove_hull(self, hull)) { 
    bumphull_free(hull);
  }
  return self;
}


/** Removes the body but does not free it. */
BumpBody * bumpworld_remove_body_only(BumpWorld * self, BumpBody * body) {
  if(!self) return NULL;
  if(!body) return NULL;  
  if (body->id < 0) return NULL;
  dynar_putptr(self->bodies, body->id, NULL);
  body->id = -1;  
  return body;
}

/** Removes the body and frees it. Does not remove it's hulls. */
BumpWorld * bumpworld_delete_body_only(BumpWorld * self, BumpBody * body) {
  if(!bumpworld_remove_body_only(self, body)) { 
    bumpbody_free(body);
  }
  return self;
}

/** Removes the body with the given index and and frees it. Does not
 * remove it's hulls. */
BumpWorld * bumpworld_delete_body_only_index(BumpWorld * self, int index) {
  BumpBody * body = bumpworld_body(self, index);  
  return bumpworld_delete_body(self, body);
}

/** Removes the hull with the given index and and frees it. */
BumpWorld * bumpworld_delete_hull_index(BumpWorld * self, int index) {
  BumpHull * hull = bumpworld_hull(self, index);  
  return bumpworld_delete_hull(self, hull);
}

/** Deletes all hulls for the given body. */
BumpWorld * bumpworld_delete_hulls_for(BumpWorld * self, BumpBody * body) {
  int hindex;
  if (!body) return NULL;

  for (hindex = 0; hindex < bumpworld_hull_count(self); hindex++) {
      BumpHull * hull = bumpworld_hull(self, hindex);
      if (hull && (hull->body == body)) {
        bumpworld_delete_hull(self, hull);
      }
  }
  return self;
}

/* Deletes the body and all hulls reated to it. */
BumpWorld * bumpworld_delete_body(BumpWorld * world, BumpBody * body) {
  if (!bumpworld_delete_hulls_for(world, body)) return NULL;
  return bumpworld_delete_body_only(world, body);
}

/* Deletes the body and all hulls related to it by it's index. */
BumpWorld * bumpworld_delete_body_index(BumpWorld * world, int index) {
  BumpBody * body = bumpworld_body(world, index);
  return bumpworld_delete_body(world, body);
}


/* Gets the tile map that this world must use to check collisions. 
 * Bumpworld doesn't own the map, and won't clean it up on bumpworld_free.
 */
Tilemap * bumpworld_tilemap(BumpWorld * self) {
  if(!self) return NULL;
  return self->map;
}


/* Sets the tile map that this world must use to check collisions. 
 * Does not clean up the tile map since bumpworld doesn't own the map.
 */
void * bumpworld_tilemap_(BumpWorld * self, void * map) {
  if(!self) return NULL;
  self->map = map;
  return map;
}

/* Calculates the motion and speed of the body 
 disregarding any collisions. v_next and p_next will be set, but p and v not.
 Call bumpbody_update_commit after collision calculation to finalize the motion. */
void bumpbody_integrate(BumpBody * self, double dt) {
  if (!self) return;
  /*
  self->a_next = bevec_add(self->a, bevec_div(self->f, self->m));
  self->v_next = bevec_add(self->v, bevec_mul(self->a_next, dt));
  */
  self->v_next = self->v;
  self->p_next = bevec_add(self->p, bevec_mul(self->v_next, dt)); 
}

/* Uses the values in p_next and v_next current position and value.  */
void bumpbody_commit(BumpBody * self) {
  if (!self) return;
  
  self->v      = self->v_next;
  self->p      = self->p_next;
}



/* Collides a hull with a single tile in the tile map. *
 */
void 
bumpworld_collide_hull_tile
(BumpWorld * self, BumpHull * hull, BumpAABB * bounds, 
 int x, int y, int z, double dt) {
  Thing * thing         = NULL;
  int tx, ty;
  tx = (x / TILE_W);
  ty = (y / TILE_H);
  // Remember, z goes first since it's a layer. 
  Tile * tile = tilemap_get(self->map, z, tx, ty);
  // No tile, no collision, sorry. 
  if (!tile) return;
  
  double   tcx          = TILE_W * tx + TILE_W / 2;
  double   tcy          = TILE_H * ty + TILE_H / 2;          
  BumpAABB tileaabb     = bumpaabb(tcx, tcy, TILE_W, TILE_H); 
  // No overlap, no collision, sorry. ;)  
  if(!bumpaabb_overlap_p((*bounds), tileaabb)) {
    return;
  } 
  
  if((hull) && (hull->body)) {
    thing = bumphull_body_data(hull);
  }
  
 
  if ( thing && (thing_z(thing) > 1) && (z == 2)  ) {
    /* There tile is in the second-to upper layer, so if the hull is there, it is supported. */
    bumphull_support_(hull, TRUE);
  }

  /* Tile in non-colliding layer. Ignore collision. */
  if ((hull->layers & (1 << (z))) != (1 << (z))) {    
    return;
  }

  // Check if the point collides with a wall
  if (tile_isflag(tile, TILE_WALL)) {
    // Calculate the pushback vector. 
    BeVec push            = 
    bumpaabb_collision_pushback((*bounds), tileaabb, hull->body->v,  dt);
    /* Add the pushback to the next motion of this hull's body */
    hull->body->p_next    = bevec_add(hull->body->p_next, push);
    bounds->p             = hull->body->p_next;
    hull->body->locks     = hull->body->locks | bevec_lock_flags(push); 
  } else if (tile_isflag(tile, TILE_STAIR) && thing) {
    /* Up to the upper level if stair is touched. */
    thing_z_(thing, 3);
    /* Give it a "support" boost so it doesn't drop down right away. */
    bumphull_support_(hull, TRUE);
  }
  
  
}


/* Calculates the displacement vector that is needed to push back 
 * to_push out of pushed. Pvec is the motion vector of the collision motion. 
 * It is used to push out the hull.
 */
void bumpworld_collide_hull_tilemap(BumpWorld * self, BumpHull * hull, double dt) {
  /* Collide the hull with the grid, setting lock flags to disallow further motion 
   in the direction. For now, no swept collisions are supported, and 
   the tile map is purely rectilinear (no diagonals yet). 
   The tilemap tiles that could overlap with the outline of the hull have to be 
   checked for collision at the future location of this hull;
   */
  BumpAABB bounds;
  int x, y, z;
  if ((!hull) || (!hull->body)) return;
  if (!self->map) return;
  /* Get bounds of hull, these will be changed. */
  bounds        = bumphull_aabb_next(hull);
  /* Reset hull support to false. */
  bumphull_support_(hull, FALSE);

  
  for (y = bumpaabb_top(&bounds); y <= (bumpaabb_down(&bounds) + 1) ; y += (TILE_H ) ) {
    for (x = bumpaabb_left(&bounds); x <= (bumpaabb_right(&bounds) + 1) ; x += (TILE_W / 2 )) {
      for (z = 0; z < tilemap_panes(self->map) ; z++) {
        bumpworld_collide_hull_tile(self, hull, &bounds, x, y, z, dt); 
      }
    }  
  }  
}

/* Perform one round of hull-to-hull collisions. */
void 
bumpworld_collide_hulls
(BumpWorld * self, int round, BumpHull * hull1, BumpHull * hull2, double dt) {
  static int serno = 0;
  BeVec    push, push1, push2, vrel;
  
  BumpAABB bounds1;
  BumpAABB bounds2;  
  int x, y, z, tx, ty;
  if ((!hull1) || (!hull1->body)) return;
  if ((!hull2) || (!hull2->body)) return;
  
  (void) self;
  
  serno++;
  bounds1       = bumphull_aabb_next(hull1);
  bounds2       = bumphull_aabb_next(hull2);

  /* No overlap, no collision. */
  if (!bumpaabb_overlap_p(bounds1, bounds2)) {
    return;
  }
  
  /* Sensors don't detect each other. */
  if (hull1->flags & BUMP_FLAG_SENSOR) {
    if (hull2->flags & BUMP_FLAG_SENSOR) {
      return;
    }
  }
  
  /* Allow the script to break off a beginning collision. */
  if ((round == 0) 
  && (!callrb_collide_hulls(hull1, hull2, COLLIDE_BEGIN, NULL))) {
    return;
  }
  
  vrel            = bevec_sub(hull1->body->v, hull2->body->v);
  push            = 
  bumpaabb_collision_pushback(bounds1, bounds2, vrel, dt);  
  /* This 0.5 should be related to the body's mass or something. */
  push1           = bevec_mul(push,  0.5);
  push2           = bevec_mul(push, -0.5);
  
  if (round == 0) {
    hull1->body->locks = 0;
    hull2->body->locks = 0;
  } else {
    push1 = bevec_lock(push1, hull1->body->locks);
    push2 = bevec_lock(push2, hull2->body->locks);
  }
  
  
  /* Sensors cause no displacement. */
  if ((!(hull1->flags & BUMP_FLAG_SENSOR)) && 
      (!(hull2->flags & BUMP_FLAG_SENSOR))
    ) 
  {
  
    hull1->body->p_next    = bevec_add(hull1->body->p_next, push1);
    hull2->body->p_next    = bevec_add(hull2->body->p_next, push2);
    hull1->body->v    = bevec_add(hull1->body->v, bevec_mul(push1, dt));
    hull2->body->v    = bevec_add(hull2->body->v, bevec_mul(push2, dt));
  }
  
  if (round == 1) {
      /* Signal the script that the collision is ongoing or ending. */
      int colltype  = COLLIDE_COLLIDING;
      bounds1       = bumphull_aabb_next(hull1);
      bounds2       = bumphull_aabb_next(hull2);
      if(!bumpaabb_overlap_p(bounds1, bounds2)) {
        colltype    = COLLIDE_END;
      }
    callrb_collide_hulls(hull1, hull2, colltype, NULL);
  }
  
}


/**
 * Adds a hull to a spatial hash. Not currently used.
 * since current collision detection still seems fast enough.
 * Perhaps add as an optimization to support really a lot of 
 * hulls later.
 */
void bumphash_addhull(BumpHash * hash, BumpHull * hull) {
  int x, y;
  BumpAABB bounds = bumphull_aabb_real(hull);
  for (y = bumpaabb_top(&bounds); y <= bumpaabb_down(&bounds) ; y += BUMPHASH_TILE_H) {
    for (x = bumpaabb_left(&bounds); x <= bumpaabb_right(&bounds) ; x += BUMPHASH_TILE_W) {
      bumphash_add(hash, x, y, hull);      
    }
  }  
}

/* Commit motions. */
BumpWorld * bumpworld_commit(BumpWorld * self) {
  int index;
  /* Commit motions. */
  for (index = 0; index < self->body_max; index ++) {
    BumpBody * body = dynar_getptr(self->bodies, index);
    bumpbody_commit(body);
  }  
  return self;
}

#undef BUMPWORLD_COMMIT_AFTER_DEBUG

/* Makes unsupported hulls fall down from upper to lower level. */
BumpWorld * bumpworld_hulls_fall_down(BumpWorld * self, double dt) {
  int index, jndex; 
  /* simple integration for now. */
  for (index = 0; index < self->body_max; index ++) {
    BumpBody * body = dynar_getptr(self->bodies, index);
    bumpbody_integrate(body, dt);
  }
  return self;
}


BumpWorld * bumpworld_update(BumpWorld * self, double dt) {
  int index, jndex; 
  /* simple integration for now. */
  for (index = 0; index < self->body_max; index ++) {
    BumpBody * body = dynar_getptr(self->bodies, index);
    bumpbody_integrate(body, dt);
  }
  /* XXX: Update the spatial hash. 
  bumphash_empty(&self->hash);
   
  for (index = 0; index < self->hull_max; index ++) {
    BumpHull * hull = dynar_getptr(self->hulls, index);
    bumphash_addhull(&self->hash, hull);    
  }
  */
  
  /* First round of hull to hull collisions. */
  for (index = 0; index < self->hull_max; index ++) {
    BumpHull * hull1 = bumpworld_hull(self, index);
    if (!hull1) continue;
    if (hull1->flags & BUMP_FLAG_DISABLED) continue;
    /* Todo: broad phase checking. */
    for (jndex = index + 1 ; jndex < self->hull_max; jndex++ )  { 
      BumpHull * hull2 = bumpworld_hull(self, index);
      if (!hull2) continue;
      if (hull2->flags & BUMP_FLAG_DISABLED) continue;
      bumpworld_collide_hulls(self, 0, hull1, hull2, dt);
    }
  }
  
  /* Hull to tile map collisions, if needed. */
  if (self->map) { 
    for (index = 0; index < self->hull_max; index ++) {
      BumpHull * hull = dynar_getptr(self->hulls, index);
      if (!hull) continue;
      if (hull->flags & BUMP_FLAG_DISABLED) continue;

      bumpworld_collide_hull_tilemap(self, hull, dt);
    }
  }
  
  /* Resolve the hull to hull collisions, taking the lock set by the grid 
   * into consideration.
   */
  for (index = 0; index < self->hull_max; index ++) {
    BumpHull * hull1 = dynar_getptr(self->hulls, index);
    if (!hull1) continue;
    if (hull1->flags & BUMP_FLAG_DISABLED) continue;

    /* Todo: broad phase checking. */
    for (jndex = index + 1 ; jndex < self->hull_max; jndex++ )  { 
      BumpHull * hull2 = dynar_getptr(self->hulls, jndex);
      if (!hull2) continue;
      if (hull2->flags & BUMP_FLAG_DISABLED) continue;

      bumpworld_collide_hulls(self, 1, hull1, hull2, dt);
    }
  }

  /* Make hulls (things) fall down from the upper levels if 
   * not supported by a tile on tile map level 3 or 4. 
   * (implement flying later) 
   */
  // bumpworld_hulls_fall_down(self, dt);

  
  /* Commit motions if not showing debug drawing. */ 
#ifndef BUMPWORLD_COMMIT_AFTER_DEBUG
  bumpworld_commit(self);
#endif  
  
  return self;
}


static void bumpaabb_draw(BumpAABB * bounds, Color color, int thickness, Camera * camera) {
  int cx         ; 
  int cy         ;
  int drawx, x, w;
  int drawy, y, h;
  
  cx          = camera_at_x(camera);
  cy          = camera_at_y(camera);    
  w           = bounds->hs.x * 2; 
  h           = bounds->hs.y * 2;
  x           = bounds->p.x - bounds->hs.x;
  y           = bounds->p.y - bounds->hs.y;  
  drawx       = x - cx;
  drawy       = y - cy;
  draw_box(drawx, drawy, w, h, color, thickness);  
}



static void bumpworld_draw_tilemap_debug(BumpTilemap * self, Camera * camera) {
 (void) self; (void) camera; 
}

static void bumphull_draw_debug(BumpWorld * world, BumpHull * self, Camera * camera) {
  int cx      ; 
  int cy      ;
  int drawx, x;
  int drawy, y;
  double drawx1, drawy1, drawx2, drawy2;
  int w, h    ;
  int t       = 2;
  BeVec next_p; 
  BumpAABB bounds;
  Color color;
  // don't draw null things.
  if(!self) return;
  cx          = camera_at_x(camera);
  cy          = camera_at_y(camera);
  bounds      = bumphull_aabb_real(self);
  w           = bounds.hs.x * 2; 
  h           = bounds.hs.y * 2;
  color       = color_rgba(0, 0, 64, 255);
  { 
    x         = bounds.p.x;
    y         = bounds.p.y;
    t         = 2;
  }
  /* Do not draw out of camera range. */
  if(!camera_cansee(camera, x, y, w, h)) {
    return;
  }
  
  drawx = x - cx;
  drawy = y - cy;
  bumpaabb_draw(&bounds, color, t, camera); 
  
  
  next_p        =  /*bevec_add( */ self->body->p_next /* , self->delta) */ ;
  bounds.p      =  next_p;
  drawy1        =  bumpaabb_top(&bounds)   - (double)cy;
  drawy2        =  bumpaabb_down(&bounds)  - (double)cy;
  drawx1        =  bumpaabb_left(&bounds)  - (double)cx;
  drawx2        =  bumpaabb_right(&bounds) - (double)cx;
  color         = color_rgba(64, 0, 128, 128);
  // al_draw_rectangle(floor(drawx1), floor(drawy1), floor(drawx2), floor(drawy2), color, 1);
  bumpaabb_draw(&bounds, color, 1, camera); 
  color         = color_rgba(255, 0, 64, 128);
  
  for (y = bumpaabb_top(&bounds); y <= (bumpaabb_down(&bounds) + 1) ; y += (TILE_H) ) {
    for (x = bumpaabb_left(&bounds); x <= (bumpaabb_right(&bounds) + 1) ; x += (TILE_W)) {
        int tx = (x / TILE_W);
        int ty = (y / TILE_H);
        Tile * tile = tilemap_get(world->map, 1, tx, ty);
        // Check if the point collides with a wall...                
        if (tile_isflag(tile, TILE_WALL)) { 
          BeVec ovec;
          double   tcx          = TILE_W * tx + TILE_W / 2;
          double   tcy          = TILE_H * ty + TILE_H / 2;          
          BumpAABB tileaabb     = bumpaabb(tcx, tcy, TILE_W, TILE_H); 
          color                 = color_rgba(255, 128, 128, 128);
          bumpaabb_draw(&tileaabb, color, 1, camera); 
          
          drawx = (tx * TILE_W)  - cx;
          drawy = (ty * TILE_H)  - cy;          
          color         = color_rgba(255, 0, 64, 128);
          draw_box(drawx, drawy, TILE_W, TILE_H, color, 1);
          ovec = bumpaabb_overlap_vector(bounds, tileaabb);
          
          color         = color_rgba(64, 0, 255, 128);
          al_draw_line(drawx, drawy, drawx + ovec.x, drawy + ovec.y, color, 1);  
          al_draw_circle(drawx + ovec.x, drawy + ovec.y, 3, color, 1);  
          
        }
        
        drawx = x - cx;
        drawy = y - cy;
        // al_draw_filled_circle(drawx, drawy, 3, color);
    }
  }
  
  
  
}

static void bumpbody_draw_debug(BumpBody * self, Camera * camera) {
  int cx      ; 
  int cy      ;
  int drawx, x;
  int drawy, y;
  int w, h    ;
  int t    = 2;
  Color color;
  // don't draw null things.
  if(!self) return;
  cx          = camera_at_x(camera);
  cy          = camera_at_y(camera);
  w           = 32;
  h           = 32;
  color       = color_rgba(128, 255, 255, 128);
  { 
    x         = self->p.x;
    y         = self->p.y;
    t         = 1;
  }
  /* Do not draw out of camera range. */
  if(!camera_cansee(camera, x, y, w, h)) {
    return;
  }
  drawx = x - cx;
  drawy = y - cy;
  draw_box(drawx - (w / 2), drawy - (h / 2), w, h, color, t);
  color = color_rgba(128, 16, 16, 128);  
  al_draw_line(drawx, drawy, drawx + self->v.x, drawy + self->v.y, color, 1);  
  al_draw_filled_circle(self->p_next.x - cx,  self->p_next.y - cy, 3, color);  
  
}


BumpWorld * bumpworld_draw_debug(BumpWorld * self) {
  int index;
  State  * state  = state_get();
  Camera * camera = state_camera(state);

  for (index = 0 ; index < self->hull_max; index ++) { 
    BumpHull * hull = dynar_getptr(self->hulls, index);
    bumphull_draw_debug(self, hull, camera);
  }
  
  for (index = 0 ; index < self->body_max; index ++) { 
    BumpBody * body = dynar_getptr(self->bodies, index);
    bumpbody_draw_debug(body, camera);
  }  
  
  /* Commit motions if showing debug drawing. */ 
#ifdef BUMPWORLD_COMMIT_AFTER_DEBUG
  bumpworld_commit(self);
#endif  

  return NULL;
}

/** Finds all hulls in a given rectangle and stores
 * up to max_hulls of them in the array hulls.
 * Returns the amount of hulls found or negative on error. */
int bumpworld_find_and_fetch_hulls(BumpWorld * self, BumpAABB search,
  BumpHull ** hulls, int max_hulls) { 
  int amount = 0;
  int index;
  int stop   = self->hull_max;
  if (stop > max_hulls) stop = max_hulls;
  if (!hulls) return -1;
    
  for (index = 0; index < stop; index ++) {
    BumpHull * hull = dynar_getptr(self->hulls, index);
    BumpAABB hullaabb;
    if (!hull) continue;
    hullaabb = bumphull_aabb_real(hull);
    if (bumpaabb_overlap_p(hullaabb, search)) {
      hulls[amount]  = hull;
      amount ++;
    }
  }
  return amount;
}

/** Finds all hulls in a given rectangle and calls the callback for each of them.
 * Returns 0. If the callback returns nonzero returns this in stead immediately
 * and stop calling the callback. */
int bumpworld_find_hulls(BumpWorld * self, BumpAABB search, void * extra,
  int (*callback)(BumpHull * hull, void * extra)) {
  int amount = 0;
  int index;

  LOG("Find hull: %d hulls known.\n", self->hull_max);
  
    
  for (index = 0; index < self->hull_max; index ++) {
    BumpHull * hull = dynar_getptr(self->hulls, index);
    BumpAABB hullaabb;
    if (!hull) continue;
    hullaabb = bumphull_aabb_real(hull);

    if (bumpaabb_overlap_p(hullaabb, search)) {
      int res = callback(hull, extra);
      if (res) return res;
    } else {
    }
    
  }
  return 0;
}

/** Finds all hulls that are conected to body and stores
 * up to max_hulls of them in the array hulls.
 * Returns the amount of hulls found or negative on error. */
int bumpworld_fetch_body_hulls(BumpWorld * self, BumpBody * body,
  BumpHull ** hulls, int max_hulls) { 
  int amount = 0;
  int index;
  int stop   = self->hull_max;
  if (stop > max_hulls) stop = max_hulls;
  if (!hulls) return -1;
    
  for (index = 0; index < stop; index ++) {
    BumpHull * hull = dynar_getptr(self->hulls, index);
    if (!hull) continue;
    if (hull->body == body) {
      hulls[amount] = hull;
      amount ++;
    }
  }
  return amount;
}

/** Finds all hulls for the given body and calls the callback for each of them.
 * Returns 0. If the callback returns nonzero, returns this in stead immediately
 * and stop calling the callback. */
int bumpworld_find_body_hulls(BumpWorld * self, BumpBody * body, void * extra,
  int (*callback)(BumpHull * hull, void * extra)) {
  int amount = 0;
  int index;

  LOG("Find hull: %d hulls known.\n", self->hull_max);
  
    
  for (index = 0; index < self->hull_max; index ++) {
    BumpHull * hull = dynar_getptr(self->hulls, index);
    if (!hull) continue;

    if (hull->body == body) {
      int res = callback(hull, extra);
      if (res) return res;
    }
  }
  return 0;
}


/** Adds a new hull attached to the given body into the physical world. */
BumpHull * 
bumpworld_new_hull(BumpWorld * self, BumpBody * body, BeVec delta, BumpAABB bounds, int layers, int kind) {
  BumpHull * hull = bumphull_newall(body, delta, bounds, layers, kind);
  return bumpworld_add_hull(self, hull);
}



/** Sets a flag on the hull indicated by index. */
int bumpworld_set_hull_flag(BumpWorld * me, int index,  int flag) {
  if (!me) return 0;
  return bumphull_set_flag(bumpworld_hull(me, index), flag);
}

/** Unsets a flag for the hull indicated by index. */
int bumpworld_unset_hull_flag(BumpWorld * me, int index,  int flag) {
  if (!me) return 0;
  return bumphull_unset_flag(bumpworld_hull(me, index), flag);
}

/** Sets all flags for the hull indicated by index. */
int bumpworld_hull_flags_(BumpWorld * me, int index,  int flags) {
  if (!me) return 0;
  return bumphull_flags_(bumpworld_hull(me, index), flags);
}

/** Gets all flags for the hull indicated by index. */
int bumpworld_hull_flags(BumpWorld * me, int index) {
  if (!me) return 0;
  return bumphull_flags(bumpworld_hull(me, index));
}




