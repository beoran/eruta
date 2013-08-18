#ifndef thing_H_INCLUDED
#define thing_H_INCLUDED

#include "camera.h"
#include "bump.h"
#include "sprite.h"


typedef struct Thing_ Thing; 

/* Thing types. Generally speaking, there are 
things that can move and those that cannot. 
Chipmunks treats them differently in the sense that
static things will all use a single (or one of 
a few) static bodies, which does not need to be 
freed when the thing isn't needed anymore.
*/
#define THING_UNUSED -1
#define THING_WALL   1
#define THING_WATER  2
#define THING_STAIR  3
#define THING_ACTOR  4
#define THING_MOBILE 5
#define THING_ZONE   6
#define THING_SEARCH 10
#define THING_ATTACK 11
#define THING_EFFECT 12

#define THING_LINKED_MAX 16



enum Thingflags_ {
  THING_FLAGS_OWNBODY          = 1,
  THING_FLAGS_OWNSHAPE         = 2,
  /* Thing doesn't change direction even when moving. */
  THING_FLAGS_LOCK_DIRECTION   = 4,
  /* Persistent things are to be copied from one map to the next.
   Especially useful (and automatcical) for PCs. */
  THING_FLAGS_PERSISTENT       = 8,
};








int thing_id(Thing * thing);
int thing_id_(Thing * thing, int newid);
Thing * thing_z_(Thing * self, int z);
int thing_setflag(Thing * self, int flag);
int thing_unsetflag(Thing * self, int flag);
int thing_flag_(Thing * self, int flag, int set);
int thing_flag(Thing * self, int flag);
Thing * thing_done(Thing * self);
Thing * thing_free(Thing * self);
Thing * thing_alloc();
Sprite * thing_sprite_(Thing * thing, Sprite * sprite);
Sprite * thing_sprite(Thing * thing);
int thing_pose_(Thing * thing, int pose);
int thing_direction_(Thing * thing, int direction);


int thing_static_p(Thing * self);



BeVec thing_p(Thing * self);
int thing_x(Thing * self);
int thing_y(Thing * self);
int thing_w(Thing * self);
int thing_h(Thing * self);
int thing_cx(Thing * self);
int thing_cy(Thing * self);
int thing_z(Thing * self);
BeVec thing_v(Thing * self);
int thing_vx(Thing * self);
int thing_vy(Thing * self);
void thing_v_(Thing * self, BeVec v);
void thing_vxy_(Thing * self, int vx, int vy);
void thing_vx_(Thing * self, int vx);
void thing_vy_(Thing * self, int vy);
void thing_p_(Thing * self, BeVec p);
void thing_deltap(Thing * self, BeVec delta);
void thing_pxy_(Thing * self, int x, int y);
void thing_x_(Thing * self, int x);
void thing_y_(Thing * self, int y);
void thing_applyforce(Thing * thing, const BeVec f);
void thing_applyimpulse(Thing * thing, const BeVec f);
void thing_resetforces(Thing * thing);
BeVec thing_sdp(Thing * self);
void thing_draw(Thing * self, Camera * camera);
int thing_direction(Thing * self);
int thing_pose(Thing * self);
int thing_poseifold_(Thing * self, int oldpose, int newpose);
void thing_update(Thing * self, double dt);

int thing_compare_for_drawing(const void * p1, const void * p2);

/* This is declared here to avoid a cyclical dependency. */
Thing * camera_track_ (Camera * self , Thing * track );


#endif




