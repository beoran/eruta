#ifndef BUMP_H_INCLUDED
#define BUMP_H_INCLUDED

#include "bevec.h"


struct BumpAABB;
typedef struct BumpAABB_        BumpAABB;
typedef struct BumpBody_        BumpBody;
typedef struct BumpHull_        BumpHull;
typedef struct BumpHoopHull_    BumpHoopHull;
typedef struct BumpBoxHull_     BumpBoxHull;
typedef struct BumpTilemap_     BumpTilemap;
typedef struct BumpWorld_       BumpWorld;

enum BumpTile_  {
  BUMP_TILE_NONE   = 0,
  BUMP_TILE_WALL   = 1 << 1,
  BUMP_TILE_WATER  = 1 << 2,
  BUMP_TILE_LEDGE  = 1 << 3,
  BUMP_TILE_STAIR  = 1 << 4,
  BUMP_TILE_PUSH   = 1 << 5,
  BUMP_TILE_NORTH  = 1 << 6,
  BUMP_TILE_SOUTH  = 1 << 7,
  BUMP_TILE_EAST   = 1 << 8,
  BUMP_TILE_WEST   = 1 << 9,
  BUMP_TILE_UP     = 1 << 10,
  BUMP_TILE_DOWN   = 1 << 11,
  BUMP_TILE_ICE    = 1 << 12,
};

typedef enum   BumpTile_        BumpTile;

/* Axes aligned bounds box. */
struct BumpAABB_ {  BeVec p; BeVec hs;  };




BumpAABB bumpaabb(double cx, double cy, double w, double h);
BumpAABB bumpaabb_make_int(int x, int y, int w, int h);

BumpBody * bumpbody_alloc();
BumpBody * bumpbody_init(BumpBody * self, BeVec p, double mass);
BumpBody * bumpbody_new(BeVec p, double mass);
BumpBody * bumpbody_done(BumpBody * self);
BumpBody * bumpbody_free(BumpBody * self);

BeVec bumpbody_p(BumpBody  * self);
BeVec bumpbody_v(BumpBody  * self);
BeVec bumpbody_a(BumpBody  * self);
void * bumpbody_data(BumpBody  * self);


BeVec bumpbody_p_(BumpBody  * self, BeVec v);
BeVec bumpbody_v_(BumpBody  * self, BeVec v);
BeVec bumpbody_a_(BumpBody  * self, BeVec v);
void bumpbody_data_(BumpBody  * self, void * data);

BeVec bumpbody_p_impulse(BumpBody  * self, BeVec v);
BeVec bumpbody_v_impulse(BumpBody  * self, BeVec v);
BeVec bumpbody_a_impulse(BumpBody  * self, BeVec v);

void bumpbody_applyforce(BumpBody  * self, BeVec v);
void bumpbody_applyimpulse(BumpBody  * self, BeVec v);
void bumpbody_resetforces(BumpBody  * self);


BumpHull *      bumphull_alloc();
BumpHull *      bumphull_initall(BumpHull * self, 
                  BumpBody * body, BeVec delta, 
                  BumpAABB bounds, int layers, int kind);

BumpHull *      bumphull_init(BumpHull * self, BumpBody * body, BumpAABB bounds);
BumpHull *      bumphull_newall(BumpBody * body, 
                  BeVec delta, BumpAABB bounds, int layers, int kind);

BumpHull *      bumphull_new(BumpBody * body, BumpAABB bounds);
BumpHull *      bumphull_done(BumpHull * self);
BumpHull *      bumphull_free(BumpHull * self);
BumpHull *      bumphull_layers_(BumpHull * hull, int layers);
int             bumphull_layers(BumpHull * hull);
BumpHull *      bumphull_kind_(BumpHull * hull, int kind);
int             bumphull_layers(BumpHull * hull);
BumpAABB bumphull_aabb(BumpHull * hull);
BumpAABB * bumphull_aabbptr(BumpHull * hull);
BumpBody * bumphull_body(BumpHull * hull);

BumpWorld * bumpworld_alloc();
BumpWorld * bumpworld_init(BumpWorld * self);
BumpWorld * bumpworld_new();
BumpWorld * bumpworld_done(BumpWorld * self);
BumpWorld * bumpworld_free(BumpWorld * self);
BumpBody * bumpworld_addbody(BumpWorld * self, BumpBody * body);
BumpBody * bumpworld_removebody(BumpWorld * self, BumpBody * body);
BumpBody * bumpworld_movebody(BumpWorld * to, BumpWorld * from, BumpBody * body);

BumpHull * bumpworld_addhull(BumpWorld * self, BumpHull * hull);
BumpHull * bumpworld_removehull(BumpWorld * self, BumpHull * hull);

void * bumpworld_tilemap_(BumpWorld * self, void * map);

BumpWorld * bumpworld_update(BumpWorld * self, double dt);

BumpWorld * bumpworld_commit(BumpWorld * self);

BumpWorld * bumpworld_draw_debug(BumpWorld * self);

int bumphull_support(BumpHull * hull);

int bumpworld_find_hulls(BumpWorld * self, BumpAABB search, void * extra,
  int callback(BumpHull * hull, void * extra));

int bumpworld_find_and_fetch_hulls(BumpWorld * self, BumpAABB search,
  BumpHull ** hulls, int max_hulls);






#endif




