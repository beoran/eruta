#include <math.h>

#include "eruta.h"
#include "camera.h"
#include "bump.h"

#include "mem.h"
#include "dynar.h"

#include "state.h"
#include "draw.h"



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
  int id;
  BumpBody  * body;
  BeVec     delta;
  BumpAABB    bounds;
  int         layers;
  int         kind;
};

#define BUMP_WORLD_MAXBODIES 1000
#define BUMP_WORLD_MAXHULLS  5000

struct BumpWorld_ {
  Dynar       * bodies;
  Dynar       * hulls;  
  int           body_count;
  int           hull_count;
  Tilemap     * map;
};





BumpAABB bumpaabb(double cx, double cy, double w, double h) {
  BumpAABB result = { bevec(cx, cy), bevec(w/2.0, h/2.0) }; 
  return result;
}


/* Allocates a body. */
BumpBody * bumpbody_alloc() { 
  return STRUCT_ALLOC(BumpBody);  
}

/* Initializes a body. */
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

/* Gets data of body. */
void * bumpbody_data(BumpBody  * self) {   
  return self->data;
}

/* Sets data of body. */
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
  return self;
  
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

/** Gets the bounds box of the bump hull. */
BumpAABB bumphull_aabb(BumpHull * hull) {
  return hull->bounds;
}

/** Gets a pointer to the the bounds box of the bump hull. */
BumpAABB * bumphull_aabbptr(BumpHull * hull) {
  return &(hull->bounds);
}


BumpHull * bumphull_free(BumpHull * self) {
  bumphull_done(self);
  return mem_free(self);
}


BumpWorld * bumpworld_alloc() {
  return STRUCT_ALLOC(BumpWorld);
}



/* Initializes a world. */
BumpWorld * bumpworld_init(BumpWorld * self) {
  if (!self) return NULL;
  self->body_count      = 0;
  self->hull_count      = 0;
  self->bodies          = dynar_newptr(BUMP_WORLD_MAXBODIES);
  self->hulls           = dynar_newptr(BUMP_WORLD_MAXHULLS);
  self->map             = NULL;
  return self;
}

/* Allocates and initializes a world. */
BumpWorld * bumpworld_new(BumpWorld * self) {
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

BumpBody * bumpworld_addbody(BumpWorld * self, BumpBody * body) {
  if(!self) return NULL;
  if(!body) return NULL;
  if(!dynar_putptr(self->bodies, self->body_count, body)) return NULL;
  body->id = self->body_count;
  self->body_count++;
  return body;
}

BumpHull * bumpworld_addhull(BumpWorld * self, BumpHull * hull) {
  if(!self) return NULL;
  if(!hull) return NULL;
  if(!dynar_putptr(self->hulls, self->hull_count, hull)) return NULL;
  hull->id = self->hull_count;
  self->hull_count++;
  return hull;
}

/* Removes the hull but does not free it. */
BumpHull * bumpworld_removehull(BumpWorld * self, BumpHull * hull) {
  if(!self) return NULL;
  if(!hull) return NULL;  
  if (hull->id < 0) return NULL;
  dynar_putptr(self->hulls, hull->id, NULL);
  hull->id = -1;  
  return hull;
}

/* Removes the body but does not free it. */
BumpBody * bumpworld_removebody(BumpWorld * self, BumpBody * body) {
  if(!self) return NULL;
  if(!body) return NULL;  
  if (body->id < 0) return NULL;
  dynar_putptr(self->bodies, body->id, NULL);
  body->id = -1;  
  return body;
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
 Call bumpbody_update_commit after collision calculation top finalize the motion. */
void bumpbody_integrate(BumpBody * self, double dt) {
  if (!self) return;
  /*
  self->a_next = bevec_add(self->a, bevec_div(self->f, self->m));
  self->v_next = bevec_add(self->v, bevec_mul(self->a_next, dt));
  */
  self->v_next = self->v;
  self->p_next = bevec_add(self->p, bevec_mul(self->v_next, dt)); 
}

/* Uses the values in p_next and v_next as */
void bumpbody_commit(BumpBody * self) {
  self->v      = self->v_next;
  self->p      = self->p_next;
}

void bumpworld_collide_hull(BumpWorld * self, BumpHull * hull, int dt) {
  /* Collide the hull with the grid, setting lock flags to disallow further motion 
   in the direction. For now, no swept collisions are supported, and 
   the tile map is purely rectilinear (no diagonals yet). 
   The tilemap tiles that could overlap with the outline of the hull have to be 
   checked for collision at the future location of this hull;
   */
  BeVec    next_p;
  BumpAABB next_box;
  if ((!hull) || (!hull->body)) return; 
  next_p        = bevec_add(hull->body->p_next, hull->delta);
  next_box.hs   = hull->bounds.hs; 
  
  
  
  
}

BumpWorld * bumpworld_update(BumpWorld * self, double dt) {
  int index; 
  /* simple integration for now. */
  for (index = 0; index < self->body_count; index ++) {
    BumpBody * body = dynar_getptr(self->bodies, index);
    bumpbody_integrate(body, dt);
  }
  /* Hull to grid collisions. */
  for (index = 0; index < self->hull_count; index ++) {
    BumpHull * hull = dynar_getptr(self->hulls, index);
    bumpworld_collide_hull(self, hull, dt);
  }
  
  /* Commit motions. */
  for (index = 0; index < self->body_count; index ++) {
    BumpBody * body = dynar_getptr(self->bodies, index);
    bumpbody_commit(body);
  }
  
  
  return self;
}


static void bumpworld_draw_tilemap_debug(BumpTilemap * self, Camera * camera) {
  
}

static void bumphull_draw_debug(BumpHull * self, Camera * camera) {
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
  w           = self->bounds.hs.x;
  h           = self->bounds.hs.y;
  color       = color_rgb(128, 255, 255);
  { 
    x         = self->bounds.p.x;
    y         = self->bounds.p.y;
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

static void bumpbody_draw_debug(BumpBody * self, Camera * camera) {
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
  w           = 32;
  h           = 32;
  color       = color_rgb(128, 255, 255);
  { 
    x         = self->p.x;
    y         = self->p.y;
    t         = 8;
  }
  /* Do not draw out of camera range. */
  if(!camera_cansee(camera, x, y, w, h)) {
    return;
  }
  drawx = x - cx;
  drawy = y - cy;
  draw_box(drawx, drawy, w, h, color, t);
  color = color_rgb(128, 16, 16);

   
  al_draw_line(drawx, drawy, drawx + self->v.x, drawy + self->v.y, color, 1);  
}

BumpWorld * bumpworld_draw_debug(BumpWorld * self) {
  int index;
  State  * state  = state_get();
  Camera * camera = state_camera(state);
  for (index = 0 ; index < self->hull_count; index ++) { 
    BumpHull * hull = dynar_getptr(self->hulls, index);
    bumphull_draw_debug(hull, camera);
  }  
  for (index = 0 ; index < self->body_count; index ++) { 
    BumpBody * body = dynar_getptr(self->bodies, index);
    bumpbody_draw_debug(body, camera);
  }  
  return NULL;
}







