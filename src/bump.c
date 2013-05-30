#include <math.h>

#include "bump.h"
#include "eruta.h"
#include "mem.h"
#include "dynar.h"
#include "camera.h"
#include "state.h"
#include "draw.h"



struct BumpBody_ {
  int id;
  BumpVec p;
  BumpVec v;
  BumpVec a;
  BumpVec p_impulse;
  BumpVec v_impulse;
  BumpVec a_impulse;
  BumpVec p_next;
  BumpVec v_next;
  BumpVec a_next;
  BumpVec f;
  float   m;
  int     locks;
  void   *data;
};


struct BumpHull_ {
  int id;
  BumpBody  * body;
  BumpVec     delta;
  BumpAABB    bounds;
  int         layers;
  int         kind;
};


struct BumpTilemap_ {
  int                map_w;
  int                map_h;
  int                tile_w;
  int                tile_h;  
  int                map_layers;
  void             * handle; 
  BumpTilemapQuery * query;
};

#define BUMP_WORLD_MAXBODIES 1000
#define BUMP_WORLD_MAXHULLS  5000

struct BumpWorld_ {
  Dynar       * bodies;
  Dynar       * hulls;  
  int           body_count;
  int           hull_count;
  BumpTilemap * map;
};



BumpVec bumpvec(double x, double y) { 
  BumpVec result = { x, y } ;
  return result;
}

BumpVec bumpvec0() { 
  return bumpvec(0.0, 0.0);
}

BumpVec bumpvec_add(BumpVec v1, BumpVec v2) { 
  return bumpvec(v1.x + v2.x, v1.y + v2.y);
}

BumpVec bumpvec_sub(BumpVec v1, BumpVec v2) { 
  return bumpvec(v1.x - v2.x, v1.y - v2.y);
}

BumpVec bumpvec_mul(BumpVec v1, double factor) { 
  return bumpvec(v1.x * factor, v1.y * factor);
}

BumpVec bumpvec_neg(BumpVec v1) { 
  return bumpvec(-v1.x, -v1.y);
}

double bumpvec_dot(BumpVec v1, BumpVec v2) { 
  return (v1.x * v2.x) + (v1.y * v2.y);
}


BumpVec bumpvec_div_unsafe(BumpVec v1, double factor) { 
  return bumpvec(v1.x / factor, v1.y / factor);
}

BumpVec bumpvec_div(BumpVec v1, double factor) { 
  if (factor == 0.0) {
    return bumpvec0();
  }
  return bumpvec_div_unsafe(v1, factor);
}


double bumpvec_lengthsq(BumpVec v1) { 
  return (v1.x * v1.x) + (v1.y * v1.y); 
}

double bumpvec_length(BumpVec v1) { 
  return sqrt((v1.x * v1.x) + (v1.y * v1.y)); 
}

BumpVec bumpvec_normalize(BumpVec v1) {
  double length = bumpvec_length(v1);
  return bumpvec_div(v1, length);
}

BumpVec bumpvec_normalize_unsafe(BumpVec v1) {
  double length = bumpvec_length(v1);
  return bumpvec_div_unsafe(v1, length);
}

BumpVec bumpvec_project(BumpVec vec , BumpVec on) {
  double dot            = bumpvec_dot(vec, on);
  double lengthsq       = bumpvec_lengthsq(on);
  double x              = on.x * dot / lengthsq;
  double y              = on.y * dot / lengthsq;
  return bumpvec(x, y);
}

BumpVec bumpvec_rightnormal(BumpVec vec) {
  return bumpvec(-vec.y, vec.x);
}

BumpVec bumpvec_leftnormal(BumpVec vec) {
  return bumpvec(vec.y, -vec.x);
}

double bumpvec_perproduct(BumpVec v1, BumpVec v2) {
  return bumpvec_dot(v1, bumpvec_rightnormal(v2));  
}

BumpVec bumpvec_forangle(double angle) {
  // Check this:
  return bumpvec(sin(angle), cos(angle));
}

double bumpvec_toangle(BumpVec v1) {
  return atan2(v1.y, v1.x);
}



BumpAABB bumpaabb(double cx, double cy, double w, double h) {
  BumpAABB result = { bumpvec(cx, cy), bumpvec(w/2.0, h/2.0) }; 
  return result;
}


BumpTilemap * bumptilemap_alloc() {
  return STRUCT_ALLOC(BumpTilemap);  
}

BumpTilemap * bumptilemap_init(
  BumpTilemap * self, void * map, int w, int h, int tw, int th, BumpTilemapQuery * query
) {
  if(!self) return NULL;
  self->map_w   = w;
  self->map_h   = h;
  self->tile_w  = tw;
  self->tile_h  = th;
  self->query   = query;
  self->handle  = map;
  return self;
}

BumpTilemap * bumptilemap_new(  
  void * map, int w, int h, int tw, int th, BumpTilemapQuery * query
) {
  return bumptilemap_init(bumptilemap_alloc(), map, w, h, tw, th, query);  
}

BumpTilemap * bumptilemap_done(BumpTilemap * self) {
  if(!self) return NULL;
  self->handle = NULL;
  self->query  = NULL;
  return self;
}

/* Frees a body. */
BumpTilemap * bumptilemap_free(BumpTilemap * self) {
  bumptilemap_done(self); 
  return mem_free(self); 
}

/* Allocates a body. */
BumpBody * bumpbody_alloc() { 
  return STRUCT_ALLOC(BumpBody);  
}

/* Initializes a body. */
BumpBody * bumpbody_init(BumpBody * self, BumpVec p, double mass) {
  if(!self) return NULL;
  self->a         = bumpvec0();
  self->a_impulse = bumpvec0();
  self->a_next    = bumpvec0();
  self->v         = bumpvec0();
  self->v_impulse = bumpvec0();
  self->v_next    = bumpvec0();
  self->p_impulse = bumpvec0();
  self->p_next    = bumpvec0();  
  self->f         = bumpvec0();
  self->p         = p;
  
  self->m         = mass;
  self->id        = -1;
  self->locks     = FALSE;
  self->data      = NULL;
  return self;  
}

/* Allocates and initializes a body. */
BumpBody * bumpbody_new(BumpVec p, double mass) {
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

/** Helper macro to generate simple struct getter functions. */
#define BAD_GETTER(MEMBERTYPE, SELFTYPE, PREFIX, MEMBER)  \
  MEMBERTYPE PREFIX##_##MEMBER(SELFTYPE self) {           \
    return self->MEMBER;                                  \
}

/** Helper macro to generate simple struct setter functions. */
#define BAD_SETTER(MEMBERTYPE, SELFTYPE, PREFIX, MEMBER)              \
  void PREFIX##_##MEMBER##_(SELFTYPE self, MEMBERTYPE value) {        \
  if (!self) return;                                                  \
  self->MEMBER = value;                                               \
}



/* Gets position of body. */
BAD_GETTER(BumpVec, BumpBody *, bumpbody, p)
/* Gets speed of body. */
BAD_GETTER(BumpVec, BumpBody *, bumpbody, v)
/* Gets accelleration of body. */
BAD_GETTER(BumpVec, BumpBody *, bumpbody, a)

/* Gets data of body. */
BAD_GETTER(void *, BumpBody *, bumpbody, data)

/* Sets data of body. */
BAD_SETTER(void *, BumpBody *, bumpbody, data)


BumpVec bumpbody_p_(BumpBody  * self, BumpVec v) {   
  return self->p = v;
}

BumpVec bumpbody_v_(BumpBody  * self, BumpVec v) {   
  return self->p = v;
}

BumpVec bumpbody_a_(BumpBody  * self, BumpVec v) {   
  return self->a = v;
}

BumpVec bumpbody_p_impulse(BumpBody  * self, BumpVec v) {  
  return self->p_impulse = v;
}

BumpVec bumpbody_v_impulse(BumpBody  * self, BumpVec v)  {  
  return self->p_impulse = v;
}

BumpVec bumpbody_a_impulse(BumpBody  * self, BumpVec v) {  
  return self->p_impulse = v;
}


void bumpbody_applyforce(BumpBody  * self, BumpVec v) {
  if (!self) return;
  self->f = bumpvec_add(self->f, v);  
}

void bumpbody_applyimpulse(BumpBody  * self, BumpVec v) {
  if (!self) return;
  self->p_impulse = bumpvec_add(self->p_impulse, v);  
}

void bumpbody_resetforces(BumpBody  * self) {
  self->f = bumpvec0();
}


BumpHull * bumphull_alloc() { 
  BumpHull  * hull = STRUCT_ALLOC(BumpHull);
  return hull;
}

/* Initializes a collision hull. */
BumpHull * bumphull_initall(
  BumpHull * self, BumpBody * body, BumpVec delta, 
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
  return bumphull_initall(self, body, bumpvec0(), bounds, 1, -1);
}

BumpHull * bumphull_newall(
  BumpBody * body, BumpVec delta, BumpAABB bounds, int layers, int kind
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
  bumptilemap_free(self->map);
  self->bodies = dynar_free(self->bodies);
  self->hulls  = dynar_free(self->hulls);
  if(self->map) { bumptilemap_free(self->map); } 
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
  if(!dynar_putptr(self->bodies, self->hull_count, hull)) return NULL;
  hull->id = self->hull_count;
  self->hull_count++;
  return hull;
}

/* Removes the hull but does not free it. */
BumpHull * bumpworld_removehull(BumpWorld * self, BumpHull * hull) {
  if(!self) return NULL;
  if(!hull) return NULL;  
  if (hull->id < 0) return NULL;
  dynar_putptr(self->bodies, hull->id, NULL);
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


/* Sets the tile map wrapper. If an old one was set, that one will be cleaned 
 * with bumptilemap_free up. */
BumpTilemap * bumpworld_tilemap_(BumpWorld * self, BumpTilemap * map) {
  if(!self) return NULL;
  if(self->map) { bumptilemap_free(self->map); } 
  self->map = map;
  return map;
}

/* Allocates a new tile map wrapper and sets it as this world's tile map. */
BumpTilemap * 
bumpworld_newtilemap(BumpWorld * self, void * map, 
                     int w, int h, int tw, int th, BumpTilemapQuery * query) {
  BumpTilemap * bumptilemap;
  bumptilemap = bumptilemap_new(map, w, h, tw, th, query);
  return bumpworld_tilemap_(self, bumptilemap);
}

void bumpbody_update(BumpBody * self, double dt) {
  if (!self) return;
  self->a_next = bumpvec_add(self->a, bumpvec_div(self->f, self->m));
  self->v_next = bumpvec_add(self->v, bumpvec_mul(self->a_next, dt));
  self->p_next = bumpvec_add(self->p, bumpvec_mul(self->v_next, dt));
  self->a      = self->a_next;
  self->v      = self->v_next;
  self->p      = self->p_next;
}

BumpWorld * bumpworld_update(BumpWorld * self, double dt) {
  int index; 
  /* simple integration for now. */
  for (index = 0; index < self->body_count; index ++) {
    BumpBody * body = dynar_getptr(self->bodies, index);
    bumpbody_update(body, dt);
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

BumpWorld * bumpworld_draw_debug(BumpWorld * self) {
  int index;
  State  * state  = state_get();
  Camera * camera = state_camera(state);
  for (index = 0 ; index < self->hull_count; index ++) { 
    BumpHull * hull = dynar_getptr(self->hulls, index);
    bumphull_draw_debug(hull, camera);
  }  
  return NULL;
}







