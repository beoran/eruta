#ifndef thing_H_INCLUDED
#define thing_H_INCLUDED


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


#include "eruta.h"
#include "sprite.h"
#include "area.h"




/**
* A Thing is any in-game object that appears the world/map view.
*/
struct Thing_ {
  int         kind;  /* What kind of thing it is. Same as collision type. */
  int         id;    /* Numercial ID. */
  int         flags; /* State flags.  */
  Area      * area; /* Area the thing is in if any. */
  cpBody    * body; /* Physical body of the thing. Is NULL for statical body. */
  cpShape   * shape; /* Main collision shape of the thing. */
  int         z; /* Layer the thing is in. */
  void *      data; /* Logical data of the thing. */
  /* Chipmunk makes it rather hard to get to the size of a 
  shape, and also since static shapes all have the same body, the position 
  of static shapes is lost. And getting the box is
  not reliable neough. So keep the size and position 
  for static shapes here even if it's slightly redundant.
  */
  Point       size; /* size of outline of shape */
  Point       spos; /* Position, merely for static shapes, for dynamic
  bodies, use cpBodyGetPos*/
  /* Sprite information. Also is the reference for the direction the thing is facing. 
   */
  SpriteState spritestate;
  /* Link back to "owner" for attacks, etc. Null if independent. */
  Thing     * owner; 
  /* Linked things, such as searchers, attacks, spells. */
  Thing     * linked[THING_LINKED_MAX];     
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


cpShape * shape_rectnew(cpBody * body, 
                        cpFloat x, cpFloat y, cpFloat w, cpFloat h, 
                        cpFloat xoff, cpFloat yoff
                        );

int thing_static_p(Thing * self);

Thing * thing_initgeneric(Thing * self, Area * area, int kind, int z,
                   cpBody * body, cpShape * shape);

Thing * thing_initstatic(Thing * self, Area * area, 
                       int kind, 
                       int x, int y, int z, int w, int h);

Thing * thing_initdynamic(Thing * self, Area * area, 
                       int kind, int x, int y, int z, int w, int h);


Thing * thing_newstatic(Area * area, 
                       int kind, 
                       int x, int y, int z, int w, int h) ;
                       
Thing * thing_newdynamic(Area * area, 
                       int kind, 
                       int x, int y, int z, int w, int h);

Point thing_p(Thing * self);
int thing_x(Thing * self);
int thing_y(Thing * self);
int thing_w(Thing * self);
int thing_h(Thing * self);
int thing_cx(Thing * self);
int thing_cy(Thing * self);
int thing_z(Thing * self);
Point thing_v(Thing * self);
int thing_vx(Thing * self);
int thing_vy(Thing * self);
void thing_v_(Thing * self, Point v);
void thing_vxy_(Thing * self, int vx, int vy);
void thing_vx_(Thing * self, int vx);
void thing_vy_(Thing * self, int vy);
void thing_p_(Thing * self, Point p);
void thing_deltap(Thing * self, Point delta);
void thing_pxy_(Thing * self, int x, int y);
void thing_x_(Thing * self, int x);
void thing_y_(Thing * self, int y);
void thing_applyforce(Thing * thing, const Point f);
void thing_applyimpulse(Thing * thing, const Point f);
void thing_resetforces(Thing * thing);
Point thing_sdp(Thing * self);
void thing_draw(Thing * self, Camera * camera);
int thing_direction(Thing * self);
int thing_pose(Thing * self);
int thing_poseifold_(Thing * self, int oldpose, int newpose);
void thing_update(Thing * self, double dt);

int thing_compare_for_drawing(const void * p1, const void * p2);




#endif




