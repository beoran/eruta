
#include "bump.h"
#include "eruta.h"
#include "mem.h"
#include "dynar.h"

#include <math.h>


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
  float   m;
  int     locks;
};


struct BumpHull_ {
  int id;
  BumpBody  body;
  BumpVec   delta;
  BumpAABB  bounds;
  int       layers;
  int       kind;
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
  BumpBody    * bodies[BUMP_WORLD_MAXBODIES];
  BumpHull    * hulls[BUMP_WORLD_MAXHULLS];  
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

BumpTilemap * bumptilemap_free(BumpTilemap * self) {
  bumptilemap_done(self); 
  return mem_free(self); 
}

BumpBody * bumpbody_alloc() { 
  return STRUCT_ALLOC(BumpBody);  
}

BumpBody * bumpbody_init(BumpBody * self, BumpVec p, double mass);
BumpBody * bumpbody_new(BumpVec p, double mass);
BumpBody * bumpbody_done(BumpBody * self);
BumpBody * bumpbody_free(BumpBody * self);

BumpVec bumpbody_p(BumpBody  * self);
BumpVec bumpbody_v(BumpBody  * self);
BumpVec bumpbody_a(BumpBody  * self);

BumpVec bumpbody_p_(BumpBody  * self, BumpVec v);
BumpVec bumpbody_v_(BumpBody  * self, BumpVec v);
BumpVec bumpbody_a_(BumpBody  * self, BumpVec v);

BumpVec bumpbody_p_impulse(BumpBody  * self, BumpVec v);
BumpVec bumpbody_v_impulse(BumpBody  * self, BumpVec v);
BumpVec bumpbody_a_impulse(BumpBody  * self, BumpVec v);


BumpHull * bumphull_alloc();
BumpHull * bumphull_initall(
  BumpHull * self, BumpBody * body, BumpVec delta, 
  BumpAABB bounds, int layers, int kind
);

BumpHull * bumphull_init(BumpHull * self, BumpBody * body, BumpAABB bounds);

BumpHull * bumphull_newall(
  BumpBody * body, BumpVec delta, BumpAABB bounds, int layers, int kind
);
BumpHull * bumphull_new(BumpBody * body, BumpAABB bounds);
BumpHull * bumphull_done(BumpHull * self);
BumpHull * bumphull_free(BumpHull * self);


BumpWorld * bumpworld_alloc();
BumpWorld * bumpworld_init(BumpWorld * self);
BumpWorld * bumpworld_new();
BumpWorld * bumpworld_done(BumpWorld * self);
BumpWorld * bumpworld_free(BumpWorld * self);
BumpBody * bumpworld_addbody(BumpWorld * self, BumpBody * body);
BumpHull * bumpworld_addhull(BumpWorld * self, BumpHull * hull);
BumpTilemap * bumpworld_tilemap_(BumpWorld * self, BumpTilemap * map);

BumpWorld * bumpworld_update(BumpWorld * self, double dt);

BumpWorld * bumpworld_draw_debug(BumpWorld * self);







