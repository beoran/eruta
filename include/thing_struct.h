#ifndef THING_STRUCT_H_INCLUDED
#define THING_STRUCT_H_INCLUDED

#include "bevec.h"
#include "area.h"
#include "bump.h"

/**
* A Thing is any in-game object that appears the world/map view.
*/
struct Thing_ {
  int         kind;  /* What kind of thing it is. Same as collision type. */
  int         id;    /* Numercial ID. */
  int         flags; /* State flags.  */
  Area      * area; /* Area the thing is in if any. */
  BumpBody  * physical; /* Physical body of the thing. Is NULL for statical body. */
  BumpHull  * hull;
  int         z; /* Layer the thing is in. */
  void *      data; /* Logical data of the thing. */
  /* Chipmunk makes it rather hard to get to the size of a 
  shape, and also since static shapes all have the same body, the position 
  of static shapes is lost. And getting the box is
  not reliable enough. So keep the size and position 
  for static shapes here even if it's slightly redundant.
  */
  BeVec       size; /* size of outline of shape */
  BeVec       spos; /* Position, merely for static shapes, for dynamic
  bodies, use cpBodyGetPos*/
  /* Sprite information. Also is the reference for the direction the thing is facing. 
   */
  SpriteState         spritestate;
  /* Link back to "owner" for attacks, etc. Null if independent. */
  struct Thing_     * owner; 
  /* Linked things, such as searchers, attacks, spells. */
  struct Thing_     * linked[THING_LINKED_MAX];
};



#endif