#ifndef area_H_INCLUDED
#define area_H_INCLUDED



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


int area_maxthings (Area * area);

Thing * area_thing (Area * area , int index);

Thing * area_thing_(Area * area , int index , Thing * set);

int area_get_unused_thing_id(Area * self);

Thing * area_addthing (Area * area , int index, Thing * thing);

int area_deletething (Area * area , int index);

Area * area_cleanupthings (Area * self);

Area * area_emptythings (Area * self);

Area * area_done (Area * self);

Area * area_free (Area * self);

Area * area_alloc(void);

Area * area_init (Area * self);

Area * area_new(void);

void area_draw_physics_(Area * self, int draw);

int area_draw_physics(Area * self);



Thing * 
area_newstatic(Area * self, int index, int kind , int x , int y , int z , int w , int h);

Thing * 
area_newdynamic(Area * self, int index, int kind , int x , int y , int z , int w , int h);

void area_draw (Area * self , Camera * camera );

void area_update (Area * self , double dt );

Thing * thing_new_dynamic(Area * area, 
                       int id, int kind, 
                       int x, int y, int z, int w, int h);

void area_draw_layer (Area * self, Camera * camera, int layer);


/* There are some tile map related functionalities that are declared in tilemap.h 
 * in stead of here to avoid cyclical dependencies. 
 */

/* Finding of things. */
int area_find_things(Area * self, int x, int y, int w, int h,
  void * extra,
  int (callback)(Thing * thing, void * extra));

BumpHull * area_add_hull(Area * self, int index, int kind,
                      int x, int y, int z, int w, int h);

Area * area_delete_hull(Area * self, int index);
Area * area_delete_body(Area * self, int index);

int area_set_thing_hull_flag(Area * me    , int index,  int flag);
int area_unset_thing_hull_flag(Area * me  , int index,  int flag);
int area_thing_hull_flags_(Area * me      , int index,  int flags);
int area_thing_hull_flags(Area * me       , int index);

int area_set_hull_flag(Area * me          , int index,  int flag);
int area_unset_hull_flag(Area * me        , int index,  int flag);
int area_hull_flags_(Area * me            , int index,  int flags);
int area_hull_flags(Area * me             , int index);

int area_thing_hull_group(Area * me , int index);
int area_thing_hull_group_(Area * me, int index, int group);


void area_update_things(Area * self);

Thing * area_new_thing(Area * self, int kind,
                        int x, int y, int z, int w, int h);                        

int area_new_thing_id(Area * self, int kind, int x, int y, int z, int w, int h);

int area_delete_thing(Area * area, int index);

Thing * bumphull_thing(BumpHull * hull);



#ifdef COMMENT_
int thing_track (Tracker * tracker , void * data );

#endif


#endif




