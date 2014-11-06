
#include "pique.h"

#include "dynar.h"
#include "thing.h"

/* PIQue is a Physical Information Queue where the area stores
 * information and events regarding the objects it manages, such as collisions,
 * combat strikes, button touches, etc.
 */


enum PeventType_ {
  PEVENT_WALK_ON,
  PEVENT_BUMP_INTO,
  PEVENT_ATTACK_HIT,
  PEVENT_ATTACK,
};


struct Pevent_ {
  int     kind;
  Thing * thing_a;
  Thing * thing_b;
  float   x, y;
  float   w, h;
  int     z;  
};

struct Pique_ {
  Dynar * events; /* Used as a circular array buffer, should be faster than a linked list.  */
  int top;
  int bottom;
};











