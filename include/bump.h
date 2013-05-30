#ifndef bump_H_INCLUDED
#define bump_H_INCLUDED


struct BumpVec;
typedef struct BumpVec_         BumpVec;
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


/* 2D Vector */
struct BumpVec_  {  double x, y;            };
/* Axes aligned bounds box. */
struct BumpAABB_ {  BumpVec p; BumpVec hs;  };

/* Point query the tile map at the given x and y coordinates, with the layers 
 * as a layers flag (1 is in layer 0, 2 is in layer 1, etc ) .
 Should Return one of the BumpTile enum values. */
typedef BumpTile BumpTilemapQuery(void * tilemap, int x, int y, int layers);



BumpVec bumpvec(double x, double y);
BumpVec bumpvec0();
BumpVec bumpvec_add(BumpVec v1, BumpVec v2);
BumpVec bumpvec_sub(BumpVec v1, BumpVec v2);
BumpVec bumpvec_mul(BumpVec v1, double factor);
#define bumpvec_mult bumpvec_mul

BumpVec bumpvec_neg(BumpVec v1);
double  bumpvec_dot(BumpVec v1, BumpVec v2);
BumpVec bumpvec_div_unsafe(BumpVec v1, double factor);
BumpVec bumpvec_div(BumpVec v1, double factor);
double  bumpvec_lengthsq(BumpVec v1);
double  bumpvec_length(BumpVec v1);
BumpVec bumpvec_normalize(BumpVec v1);
BumpVec bumpvec_normalize_unsafe(BumpVec v1);
BumpVec bumpvec_project(BumpVec vec , BumpVec on);
BumpVec bumpvec_rightnormal(BumpVec vec);
BumpVec bumpvec_leftnormal(BumpVec vec);
double  bumpvec_perproduct(BumpVec v1, BumpVec v2);
BumpVec bumpvec_forangle(double angle);
double bumpvec_toangle(BumpVec v1);

BumpAABB bumpaabb(double cx, double cy, double w, double h);


BumpTilemap * bumptilemap_alloc();

BumpTilemap * bumptilemap_init(
  BumpTilemap * self, void * map, int w, int h, 
  int tw, int th, BumpTilemapQuery * query
);

BumpTilemap * bumptilemap_new(
  void * map, int w, int h, int tw, int th, BumpTilemapQuery * query
);

BumpTilemap * bumptilemap_done(BumpTilemap * self);
BumpTilemap * bumptilemap_free(BumpTilemap * self);

BumpBody * bumpbody_alloc();
BumpBody * bumpbody_init(BumpBody * self, BumpVec p, double mass);
BumpBody * bumpbody_new(BumpVec p, double mass);
BumpBody * bumpbody_done(BumpBody * self);
BumpBody * bumpbody_free(BumpBody * self);

BumpVec bumpbody_p(BumpBody  * self);
BumpVec bumpbody_v(BumpBody  * self);
BumpVec bumpbody_a(BumpBody  * self);
void * bumpbody_data(BumpBody  * self);


BumpVec bumpbody_p_(BumpBody  * self, BumpVec v);
BumpVec bumpbody_v_(BumpBody  * self, BumpVec v);
BumpVec bumpbody_a_(BumpBody  * self, BumpVec v);
void bumpbody_data_(BumpBody  * self, void * data);

BumpVec bumpbody_p_impulse(BumpBody  * self, BumpVec v);
BumpVec bumpbody_v_impulse(BumpBody  * self, BumpVec v);
BumpVec bumpbody_a_impulse(BumpBody  * self, BumpVec v);

void bumpbody_applyforce(BumpBody  * self, BumpVec v);
void bumpbody_applyimpulse(BumpBody  * self, BumpVec v);
void bumpbody_resetforces(BumpBody  * self);


BumpHull *      bumphull_alloc();
BumpHull *      bumphull_initall(BumpHull * self, 
                  BumpBody * body, BumpVec delta, 
                  BumpAABB bounds, int layers, int kind);

BumpHull *      bumphull_init(BumpHull * self, BumpBody * body, BumpAABB bounds);
BumpHull *      bumphull_newall(BumpBody * body, 
                  BumpVec delta, BumpAABB bounds, int layers, int kind);

BumpHull *      bumphull_new(BumpBody * body, BumpAABB bounds);
BumpHull *      bumphull_done(BumpHull * self);
BumpHull *      bumphull_free(BumpHull * self);
BumpHull *      bumphull_layers_(BumpHull * hull, int layers);
int             bumphull_layers(BumpHull * hull);
BumpHull *      bumphull_kind_(BumpHull * hull, int kind);
int             bumphull_layers(BumpHull * hull);
BumpAABB bumphull_aabb(BumpHull * hull);
BumpAABB * bumphull_aabbptr(BumpHull * hull);


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

BumpTilemap * bumpworld_tilemap_(BumpWorld * self, BumpTilemap * map);

BumpTilemap * 
bumpworld_newtilemap(BumpWorld * self, void * map, 
                     int w, int h, int tw, int th, BumpTilemapQuery * query);

BumpWorld * bumpworld_update(BumpWorld * self, double dt);

BumpWorld * bumpworld_draw_debug(BumpWorld * self);










#endif




