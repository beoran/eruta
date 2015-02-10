#ifndef ERUTA_STATE_H
#define ERUTA_STATE_H

/** The data struct contains all global state and other data of the application.
*/
typedef struct State_ State;


#include "eruta.h"
#include "tilemap.h"
#include "mode.h"
#include "rh.h"
#include "sprite.h"

#define STATE_COLORS   16
#define STATE_BLACK   0
#define STATE_WHITE   1
/** Amount of samples that can be played at the same time. */
#define STATE_SAMPLES 16


State * state_get(void);
State * state_set (State * state );
Tilemap * state_nowmap (State * state );
Tilemap * state_loadmap (State * state );
State * state_alloc(void);
void state_free (State * self );
State * state_errmsg_ (State * state , char * mesg );
char * state_errmsg (State * state );
State * state_eventsource (State * state , ALLEGRO_EVENT_SOURCE * src);
ALLEGRO_COLOR state_color (State * state , int color );
ALLEGRO_COLOR
  state_color_f(State * state , int color ,
    float r , float g , float b , float a );

Ruby * state_ruby (State * state );
BBConsole * state_console (State * state );
int state_initjoystick (State * self );
State * state_init (State * self , BOOL fullscreen );
BOOL state_done (State * state );
BOOL state_busy (State * self );
int state_poll (State * state , ALLEGRO_EVENT * event );
ALLEGRO_EVENT * state_pollnew (State * state );
ALLEGRO_FONT * state_font (State * state );
void state_frames_update (State * state );
int state_frames (State * state );
double state_fps (State * state );
double state_frametime (State * state );
Camera * state_camera (State * state );

Sprite * state_newsprite(State * state, int index);
Sprite * state_sprite(State * state, int index);
Sprite * state_getornewsprite(State * state, int index);

int state_sprite_load_builtin
(State * state, int sprite_index, int layer_index, char * vpath, int layout);

int state_sprite_tintlayer
(State * state, int sprite_index, int layer_index, int, int g, int b, int a);

Area * state_area(State * state);
SpriteList * state_sprites(State * state);


Thing * state_thing(State * state, int index);
Thing * state_newthing(State * state, int index, int kind,
                        int x, int y, int z, int w, int h);

int state_camera_track_(State * state, int thing_index);
int state_lockin_maplayer(State * state, int layer);
int state_loadtilemap_vpath(State * self, char * vpath);

void state_draw(State * self);
void state_flip_display(State * self);
void state_update(State * self);

int state_newthingindex(State * state, int index, int kind, 
                        int x, int y, int z, int w, int h);
int state_thing_sprite_(State * state, int thing_index, int sprite_index); 
int state_thing_pose_(State * state, int thing_index, int pose);
int state_thing_direction_(State * state, int thing_index, int direction);

int state_actorindex_(State * self, int thing_index);
Thing * state_actor(State * self);

int state_active_map_id(State * state);
int state_active_map_id_(State * state, int index);

int state_image_mask_to_alpha(State * state, int store_index, int r, int g, int b);
int state_image_average_to_alpha(State * state, int store_index, int r, int g, int b);

int global_state_show_fps();
int global_state_show_fps_(int show);
int global_state_show_graph();
int global_state_show_graph_(int show);
int global_state_show_area();
int global_state_show_area_(int show);
int global_state_show_physics();
int global_state_show_physics_(int show);

int state_get_unused_thing_id(int minimum);
int state_get_unused_sprite_id(int minimum);



#endif
