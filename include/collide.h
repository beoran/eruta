#ifndef collide_H_INCLUDED
#define collide_H_INCLUDED

#include "thing.h"

enum CollisionKinds_ {
  COLLIDE_BEGIN     = 1,
  COLLIDE_COLLIDING = 2,
  COLLIDE_END       = 3
};

int collide_things(Thing * t1, Thing * t2, int kind, void * data);



#endif




