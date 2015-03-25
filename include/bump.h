#ifndef BUMP_H_INCLUDED
#define BUMP_H_INCLUDED

#include "bevec.h"
#include "bumpshape.h"


typedef struct BumpBody_        BumpBody;
typedef struct BumpHull_        BumpHull;
typedef struct BumpHoopHull_    BumpHoopHull;
typedef struct BumpBoxHull_     BumpBoxHull;
typedef struct BumpTilemap_     BumpTilemap;
typedef struct BumpWorld_       BumpWorld;

enum BumpTile_  {
  BUMP_TILE_NONE      = 0,
  BUMP_TILE_WALL      = 1 << 1,
  BUMP_TILE_WATER     = 1 << 2,
  BUMP_TILE_LEDGE     = 1 << 3,
  BUMP_TILE_STAIR     = 1 << 4,
  BUMP_TILE_PUSH      = 1 << 5,
  BUMP_TILE_NORTH     = 1 << 6,
  BUMP_TILE_SOUTH     = 1 << 7,
  BUMP_TILE_EAST      = 1 << 8,
  BUMP_TILE_WEST      = 1 << 9,
  BUMP_TILE_UP        = 1 << 10,
  BUMP_TILE_DOWN      = 1 << 11,
  BUMP_TILE_ICE       = 1 << 12,
};

typedef enum   BumpTile_        BumpTile;

enum BumpFlag_ {
  /* Default hull flags. */
  BUMP_FLAG_NORMAL    = 0,
  /* Sensor hulls detect collisions but don't push or get pushed. */
  BUMP_FLAG_SENSOR    = 1 << 1,
  /* Flag to mark attack hulls. */
  BUMP_FLAG_DISABLED  = 1 << 2,
  /* User defined flags start from this value */
  BUMP_FLAG_USER1     = 1 << 16,
  BUMP_FLAG_USER2     = 1 << 17,
  BUMP_FLAG_USER3     = 1 << 18,
  BUMP_FLAG_USER4     = 1 << 19,
};

enum BumpKind_ {
  BUMP_KIND_PLAYER    = 1,
  BUMP_KIND_NPC       = 2,
  BUMP_KIND_FOE       = 3,
  BUMP_KIND_ATTACK    = 128,
  /* Flag to mark search hulls. */
  BUMP_KIND_SEARCH    = 129,
  /* Flag to mark art or technique hulls. */
  BUMP_KIND_ART       = 130,
  /* Disabled hulls are temporary disabled and do nothing. 
   * They may show up in hull searches, though. */
};

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

int bumphull_flags(BumpHull * hull);
int bumphull_flags_(BumpHull * hull, int flags);
int bumphull_get_flag(BumpHull * hull, int flag);
int bumphull_set_flag(BumpHull * hull, int flag);
int bumphull_unset_flag(BumpHull * hull, int flag);

BumpAABB bumphull_aabb(BumpHull * hull);
BumpAABB * bumphull_aabbptr(BumpHull * hull);
BumpBody * bumphull_body(BumpHull * hull);

BumpWorld * bumpworld_alloc();
BumpWorld * bumpworld_init(BumpWorld * self);
BumpWorld * bumpworld_new();
BumpWorld * bumpworld_done(BumpWorld * self);
BumpWorld * bumpworld_free(BumpWorld * self);
BumpBody * bumpworld_movebody(BumpWorld * to, BumpWorld * from, BumpBody * body);

BumpHull * bumpworld_hull(BumpWorld * self, int index);
BumpBody * bumpworld_body(BumpWorld * self, int index);
int bumpworld_body_count(BumpWorld * self);
int bumpworld_hull_count(BumpWorld * self);
BumpBody * bumpworld_add_body(BumpWorld * self, BumpBody * body);
BumpHull * bumpworld_add_hull(BumpWorld * self, BumpHull * hull);
BumpHull * bumpworld_new_hull(BumpWorld * self, BumpBody * body, BeVec delta, BumpAABB bounds, int layers, int kind);

BumpHull * bumpworld_remove_hull(BumpWorld * self, BumpHull * hull);
BumpBody * bumpworld_remove_body_only(BumpWorld * self, BumpBody * body);

BumpWorld * bumpworld_delete_hull(BumpWorld * self, BumpHull * hull);
BumpWorld * bumpworld_delete_body_only(BumpWorld * self, BumpBody * body);
BumpWorld * bumpworld_delete_body_only_index(BumpWorld * self, int index);
BumpWorld * bumpworld_delete_hull_index(BumpWorld * self, int index);
BumpWorld * bumpworld_delete_hulls_for(BumpWorld * self, BumpBody * body);
BumpWorld * bumpworld_delete_body(BumpWorld * world, BumpBody * body);
BumpWorld * bumpworld_delete_body_index(BumpWorld * world, int index);



void * bumpworld_tilemap_(BumpWorld * self, void * map);

BumpWorld * bumpworld_update(BumpWorld * self, double dt);

BumpWorld * bumpworld_commit(BumpWorld * self);

BumpWorld * bumpworld_draw_debug(BumpWorld * self);

int bumphull_support(BumpHull * hull);

int bumpworld_find_hulls(BumpWorld * self, BumpAABB search, void * extra,
  int (*callback)(BumpHull * hull, void * extra));

int bumpworld_find_and_fetch_hulls(BumpWorld * self, BumpAABB search,
  BumpHull ** hulls, int max_hulls);

int bumpworld_fetch_body_hulls(BumpWorld * self, BumpBody * body,
  BumpHull ** hulls, int max_hulls);
int bumpworld_find_body_hulls(BumpWorld * self, BumpBody * body, void * extra,
  int (*callback)(BumpHull * hull, void * extra));

int bumpworld_set_hull_flag(BumpWorld * me, int index,  int flag);
int bumpworld_unset_hull_flag(BumpWorld * me, int index,  int flag);
int bumpworld_hull_flags_(BumpWorld * me, int index,  int flags);
int bumpworld_hull_flags(BumpWorld * me, int index);

#endif




