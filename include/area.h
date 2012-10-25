#ifndef area_H_INCLUDED
#define area_H_INCLUDED

#include <chipmunk.h>

/**
An area is an in game region that forms a single unity in which play
takes place. Ok, its' what you'd call a "level". Every area
consists of a single tile map that determines the visual looks of the area,
a chipmunk cpSpace that simulates the dynamics, sprite information
that determines the visibility and position of the sprites, and a reference
to the scripting engine that contains the logical state of the game.

Division of the data locations: visual and physics engine: in C.
Logic/game/character data: in scripting engine.
*/
struct Area_;
typedef struct Area_ Area;


#include "area_proto.h"


#endif




