#ifndef collide_H_INCLUDED
#define collide_H_INCLUDED

#include "thing.h"

enum CollisionKinds_ {
  COLLIDE_BEGIN     = 1,
  COLLIDE_PRESOLVE  = 2,
  COLLIDE_POSTSOLVE = 3,
  COLLIDE_SEPARATE  = 4
};

int collide_things(Thing * t1, Thing * t2, int kind, void * data);



#endif




