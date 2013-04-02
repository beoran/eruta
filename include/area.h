#ifndef area_H_INCLUDED
#define area_H_INCLUDED

#include <chipmunk.h>



/*
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


/** Need the camera and it's prototypes. */
#include "camera.h"
#include "thing.h"


int area_maxthings (Area * area );

Thing * area_thing (Area * area , int index );

Thing * area_thing_ (Area * area , int index , Thing * set );

int area_thingid (Area * self );

Thing * area_addthing (Area * area , Thing * thing );

cpBody * area_staticbody (Area * area );

int area_deletething (Area * area , int index );

Area * area_cleanupthings (Area * self );

Area * area_emptythings (Area * self );

Area * area_done (Area * self );

Area * area_free (Area * self );

Area * area_alloc(void);

Area * area_init (Area * self );

Area * area_new(void);

cpSpace * area_space (Area * self );

Thing * area_newstatic (Area * self , int kind , int x , int y , int z , int w , int h );

Thing * area_newdynamic (Area * self , int kind , int x , int y , int z , int w , int h );

void area_draw (Area * self , Camera * camera );

void area_update (Area * self , double dt );


#ifdef COMMENT_
int thing_track (Tracker * tracker , void * data );

#endif


#endif




