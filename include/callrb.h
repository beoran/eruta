#ifndef collide_H_INCLUDED
#define collide_H_INCLUDED

#include "thing.h"

enum CollisionKinds_ {
  COLLIDE_BEGIN     = 1,
  COLLIDE_COLLIDING = 2,
  COLLIDE_END       = 3
};

int callrb_collide_things(Thing * t1, Thing * t2, int kind, void * data);

int callrb_collide_hulls(BumpHull * h1, BumpHull * h2, int kind, void * data);

/* Sprite event handler. Calls an mruby callback. */
int callrb_sprite_event(SpriteState * spritestate, int kind, void * data);



#endif




