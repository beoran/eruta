#include "eruta.h"
#include "mem.h"
#include "state.h"
#include "camera.h"
#include "sound.h"
#include "tilemap.h"
#include "tileio.h"
#include "dynar.h"
#include "mode.h"
#include "fifi.h"
#include "rh.h"
#include "toruby.h"
#include "event.h"
#include "widget.h"
#include "area.h"
#include "thing.h"
#include "sprite.h"

/* The data struct contains all global state and other data of the application.
*/
struct State_ {
  BOOL                  busy;
  BOOL                  fullscreen;
  BOOL                  audio;
  int32_t               modeno;
  // there are two fonts for now, ttf and font. Font is a plain font
  // for emergency use, ttf is the normal font.
  ALLEGRO_FONT        * font;
  ALLEGRO_FONT        * ttf;
  ALLEGRO_COLOR         colors[STATE_COLORS];
  ALLEGRO_DISPLAY     * display;
  ALLEGRO_BITMAP      * bkg;
  ALLEGRO_EVENT_QUEUE * queue;
  char                * errmsg;
  double                fpsnow, fpstime, fps;
  int                   frames;
  Tilemap             * nowmap;     // active tile map
  Tilemap             * loadingmap; // new tile map that's being loaded.
  Tilemap             * worldmap; /* World map that stays loaded. */
  Mode                * mode; // active mode
  Dynar               * modes;
  Camera              * camera;
  Area                * area;
  /* Logical and physical game objects. */
  SpriteList          * sprites;
  Ruby                * ruby;
  BBConsole           * console; 
  // The ruby and error message GUI console.
  /* The curtrent actor, controlled by the player. */
  Thing               * actor;
  
};


static State * global_state_ = NULL;

/** Gets the global state data, or NULL if not set. */
State * state_get() {
  return global_state_;
}

/** Sets the given state data as the global state data for this apllication.
* returns the *previous* state data pointer or NULL if it was not set yet.
*/
State * state_set(State * state) {
  State * oldstate    = global_state_;
  global_state_ = state;
  return oldstate;
}

/** Return's the state's active tile map. */
Tilemap * state_nowmap(State * state) {
  if(!state) return NULL;
  return state->nowmap;
}

/** Return's the state's loading tile map. */
Tilemap * state_loadingmap(State * state) {
  if(!state) return NULL;
  return state->loadingmap;
}

/* Returns the state's area. */
Area * state_area(State * state) {
  if(!state) return NULL;
  return state->area;
}

/* Return's the state's sprite list */
SpriteList * state_sprites(State * state) {
  if(!state) return NULL;
  return state->sprites;
}


/** Allocates a state struct */
State * state_alloc() {
  return STRUCT_ALLOC(State);
}

/** Frees a state struct. */
void state_free(State * self) {
  spritelist_free(self->sprites);
  self->sprites = NULL;
  area_free(self->area);
  self->area    = NULL;
  if (self->nowmap) {
    tilemap_free(self->nowmap);
    self->nowmap = NULL;
  }
  dynar_free(self->modes);
  rh_free(self->ruby);
  bbconsole_free((BBWidget *)self->console, NULL);
  self->console = NULL; // disable console immediately.
  font_free(self->font);
  al_destroy_display(self->display);
  camera_free(self->camera);
  audio_stop();
  al_uninstall_system();
  STRUCT_FREE(self);
}

/** Sets error message for state and returns NULL. */
State * state_errmsg_(State * state, char * mesg) {
  state->errmsg = mesg;
  return NULL;
}

/** Gets error message for state. */
char * state_errmsg(State * state) {
  return state->errmsg;
}


/** Registers an event source for this state */
State * state_eventsource(State * state, ALLEGRO_EVENT_SOURCE * src)  {
  al_register_event_source(state->queue, src);
  return state;
}

/** Gets a color from the state' color list. NOT bound checked! */
ALLEGRO_COLOR state_color(State * state, int color) {
  return state->colors[color];
}

/** Sets a color to the state' color list. NOT bound checked! */
ALLEGRO_COLOR state_color_f(State * state, int color,
                         float r, float g, float b, float a) {
  return state->colors[color] = al_map_rgba_f(r, g, b, a);
}


/** Gets Ruby interpreter for state. */
Ruby * state_ruby(State * state) {
  return state->ruby;
}

/** Gets console intepreter for state. */
BBConsole * state_console(State * state) {
  return state->console;
}

/* Creates a new sprite in the sprite list. */
Sprite * state_newsprite(State * state, int index) {
  SpriteList * list = state_sprites(state);
  return spritelist_newsprite(list, index);
}

/* Returns a sprite from the state's sprite list. */
Sprite * state_sprite(State * state, int index) {
  SpriteList * list = state_sprites(state);
  return spritelist_sprite(list, index);
}

/* Returns a sprite from the state's sprite list or makes it new if 
 * it didn't exist. */
Sprite * state_getornewsprite(State * state, int index) {
  SpriteList * list = state_sprites(state);
  return spritelist_getornew(list, index);
}

/* Loads a layer of a sprite from a vpath. Sprite layer is in 
 ulpcss format. */
int state_sprite_loadulpcss
(State * state, int sprite_index, int layer_index, char * vpath) { 
  Sprite * sprite = state_getornewsprite(state, sprite_index);
  if(!sprite) return -1;
  if(sprite_loadlayer_ulpcss_vpath(sprite, layer_index, vpath, 0)) { 
    return sprite_index;
  } else {
    return -2;
  }
}

/*  Gets a thing from the state's active tile map/area. */
Thing * state_thing(State * state, int index) {
  Tilemap * map = state_nowmap(state);
  return tilemap_thing(map, index);
}

/* Makes a new dynamic thing in the state's active tile map/area. */
Thing * state_newthing(State * state, int index, int kind, 
                        int x, int y, int z, int w, int h) {
  Tilemap * map = state_nowmap(state);
  return tilemap_addthing(map, index, kind, x, y, z, w, h);
}

/* Makes a new dynamic thing and returns it's index, or 
 negative if it could not be created. */
int state_newthingindex(State * state, int index, int kind, 
                        int x, int y, int z, int w, int h) {
                          
  Thing * thing = state_newthing(state, index, kind, x, y, z, w, h);
  if (!thing) return -1;
  return thing_id(thing);
}


/* Looks op both the Thing and the Sprite by index and 
 * links the Thing to the Sprite. Returns the sprite index set or negative on error.
 * Both sprite and thing must already exist for this to work. 
 */
int state_thing_sprite_(State * state, int thing_index, int sprite_index) {
  Thing * thing; Sprite * sprite;
  thing  = state_thing(state, thing_index);
  if (!thing) { return -1; } 
  sprite = state_sprite(state, sprite_index);
  if (!sprite) { return -2; }
  thing_sprite_(thing, sprite);
  return sprite_index;
}

/* Looks up the thing by index and set it's pose. Returns negative on error, 
 or if sucessful, the pose set. */
int state_thing_pose_(State * state, int thing_index, int pose) {
  Thing * thing;
  thing = state_thing(state, thing_index);
  if(!thing) return -1;
  thing_pose_(thing, pose);
  return pose;
}
 
/* Looks up the thing by index and set it's direction. Returns negative on error, 
 or if sucessful, the direction set. */
int state_thing_direction_(State * state, int thing_index, int direction) {
  Thing * thing;
  thing = state_thing(state, thing_index);
  if(!thing) return -1;
  thing_direction_(thing, direction);
  return direction;
}
 
/* Looks up the thing by index and let the state's camera track it. */
int state_camera_track_(State * state, int thing_index) {
  Thing * thing;
  thing = state_thing(state, thing_index);
  if(!thing) return -1;
  camera_track_(state_camera(state), thing);
  thing_direction_(thing, thing_index);
  return thing_index;
}


/* Sets up the state's camera to track the numbered thing. */
int state_cameratrackthing(State * state, int thing_index) {
  Thing * thing = state_thing(state, thing_index);
  if (thing) {
  	camera_track_(state_camera(state), thing);
	return 0;
  }
  return -1;
}

/* Lock in the state's camera to the current active tile map's given layer. */
int state_lockin_maplayer(State * state, int layer) {
  tilemap_layer_lockin(state_nowmap(state), layer, state_camera(state));
  return 0; 
}

/* Loads a named tile map from the map folder. */
int state_loadtilemap_vpath(State * self, char * vpath) {
  TilemapLoadExtra extra;
  extra.area = self->area;
  self->loadingmap = fifi_load_vpath(tilemap_fifi_load, &extra, vpath);
  if(!self->loadingmap) return -1;
  /* TODO: some preproscessing, and move the PCs from the old map to the new... */
  tilemap_free(self->nowmap);
  self->nowmap = self->loadingmap;
  return 0;
}


/* Sets the state current active actor to the thing with the given index.
 * Does not change if no such thing is found;
 */
int state_actorindex_(State * self, int thing_index) {
  Thing * thing;
  thing = state_thing(self, thing_index);
  if(!thing) return -1;
  self->actor = thing;
  return thing_index;
}

/* Returns the current active actor of the state. */
Thing * state_actor(State * self) {
  if(!self) return NULL;
  return self->actor;
}




#define STATE_MODES 10

int state_initjoystick(State * self) {
  int num, index, snum, sindex, bnum, bindex, anum, aindex;
  if(!al_install_joystick()) return FALSE;
  num = al_get_num_joysticks();
  printf("Found %d joysticks:\n", num);
  for(index = 0; index < num; index ++) {
    ALLEGRO_JOYSTICK * joy = al_get_joystick(index);
    if(!al_get_joystick_active(joy)) continue;
    printf("Joystick nr %d, name: %s,", index, al_get_joystick_name(joy));
    snum = al_get_joystick_num_sticks(joy);
    printf("\n%d sticks: ", snum);
    for(sindex = 0; sindex < snum; sindex++) {
      printf("%s, ", al_get_joystick_stick_name(joy, sindex));
      anum = al_get_joystick_num_axes(joy, sindex); 
      printf("%d axes: ", anum);
      for (aindex = 0; aindex < anum; aindex++) {
        printf("%s, ", 
               al_get_joystick_axis_name(joy, sindex, aindex));
      }
    }
  
    bnum = al_get_joystick_num_buttons(joy);
    printf("\n%d buttons: ", bnum);
    for(bindex = 0; bindex < bnum; bindex++) {
      printf("%s, ", al_get_joystick_button_name(joy, bindex));
    }
    printf(".\n");
  }
  printf("\n");
  return num;
}



/** Initializes the state. It opens the screen, keyboards,
 interpreter, etc. Get any error with state_errmsg if
this returns NULL. */
State * state_init(State * self, BOOL fullscreen) {
  if(!self) return NULL;
  int flags        = 0;
  self->busy       = TRUE;
  self->fullscreen = fullscreen;
  self->audio      = FALSE;
  state_errmsg_(self, "OK!");
  // Initialize Ruby scripting 
  self->ruby = rh_new();
  if(!self->ruby) {
    return state_errmsg_(self, "Could not init Ruby.");
  }
  tr_init(self->ruby);
  
  // Initialize Allegro 5 and addons
  if (!al_init()) {
    return state_errmsg_(self, "Could not init Allegro.");
  }
  al_init_image_addon();
  al_init_font_addon();
  al_init_primitives_addon();
  if(!al_init_ttf_addon()) {
    return state_errmsg_(self, "Could not init TTF extension.\n");
  }

  // Install the keyboard handler
  if (!al_install_keyboard()) {
    return state_errmsg_(self, "Error installing keyboard.\n");
  }
  
  // install mouse handler   
  if (!al_install_mouse()) {
    return state_errmsg_(self, "Error installing mouse.\n");
  }
  
  // install joystick 
  if(!state_initjoystick(self)) {
    perror("Joysticks not started.");
  }

  if(!audio_start()) {
    perror("Sound not started.");
  }

  // Use full screen mode if needed.
  if(self->fullscreen) { 
    flags = ALLEGRO_FULLSCREEN | ALLEGRO_GENERATE_EXPOSE_EVENTS;
  } 
  // flags |= ALLEGRO_OPENGL;
 
  al_set_new_display_flags(flags);
  // Create a window to display things on: 640x480 pixels.
  self->display = al_create_display(SCREEN_W, SCREEN_H);
  if (!self->display) {
    return state_errmsg_(self, "Error creating display.\n");
  }
  
  al_resize_display(self->display, SCREEN_W, SCREEN_H);
  
  // initialize the file finder, so we can start to load the data files
  if(!fifi_init()) { 
    return state_errmsg_(self, "Could not find data folder!\n");
  }

  // Tuffy.ttf
  // "OpenBaskerville-0.0.53.otf"
  #define STATE_FONTNAME "GranaPadano.ttf"
  
  self->font = fifi_loadfont(STATE_FONTNAME, 16, 0);
  if (!self->font) {
    return state_errmsg_(self, "Error loading " STATE_FONTNAME);
  }

  state_color_f(self, STATE_WHITE, 1, 1, 1, 1);
  state_color_f(self, STATE_BLACK, 0, 0, 0, 1);
  // Start the event queue to handle keyboard input and our timer
  self->queue = al_create_event_queue();  
  state_eventsource(self, al_get_keyboard_event_source());
  state_eventsource(self, al_get_display_event_source(self->display));
  state_eventsource(self, al_get_mouse_event_source());
  state_eventsource(self, al_get_joystick_event_source());
  al_set_window_title(self->display, "Eruta!");
  // set up fps counter. 
  self->fps        = 0.0;
  self->fpstime    = al_get_time();
  self->frames     = 0;  
  self->loadingmap = NULL;
  self->nowmap     = NULL;
  
  // set up modes
  self->modes   = dynar_new(ERUTA_MODE_MAX, sizeof(Mode *));
  if(!self->modes) {
    return state_errmsg_(self, "Out of memory when allocating modes.");
  }
  
  // set up camera
  self->camera = camera_new(-100, -100, SCREEN_W, SCREEN_H);
  if(!self->camera) {
      return state_errmsg_(self, "Out of memory when allocating camera.");
  }
  /* Set up console. */
  {
    Style   style = { color_rgb(255,255,255), color_rgba(64,0,0, 191), 
                      self->font, NULL};
    Rebox  bounds = { {20, 20} , {600, 400} }; 
    self->console = bbconsole_new(1, bounds, style);
    if(!self->console) {
      return state_errmsg_(self, "Out of memory when allocating console.");
    }
  }
  bbconsole_puts(self->console, "BBConsole started ok!");
  // set up ruby callback for console commands 
  bbconsole_command_(self->console, 
                   (BBConsoleCommand *)rh_dostring_console, self->ruby);
  
  /* Initialize Area. */
  self->area = area_new();
  
  /* Initialize sprite list. */
  self->sprites = spritelist_new();
  
  return self;
}

/** Sets the state's busy status to false */
BOOL state_done(State * state) {
  state->busy = FALSE;
  return state->busy;
}

/** Returns true if the state is busy false if not. */
BOOL state_busy(State * self) {
  return self->busy;
}


/* Draws all inside the state that needs to be drawn. */
void state_draw(State * self) {
    if (self->nowmap) {
       tilemap_draw(self->nowmap, self->camera);    
    }
    if (self->area) {
      area_draw(self->area, self->camera);
    }
    
    // draw fps  
    al_draw_textf(state_font(self), COLOR_WHITE,
                        10, 10, 0, "FPS: %lf, %d", state_fps(self), 
                        state_frames(self));
    // alpsshower_draw(&shower, state_camera(state));
    // draw the console (will autohide if not active).
    bbwidget_draw((BBWidget *)state_console(self));
}


/* Updates the state's display. */
void state_flip_display(State * self) {
  al_flip_display();
  state_frames_update(self);
}

/* Updates the state's elements. */
void state_update(State * self) { 
  mrb_value mval;
  // alpsshower_update(&shower, state_frametime(state));    
  if (self->nowmap) tilemap_update(self->nowmap, state_frametime(self));
  if (self->area) {
      area_update(self->area, state_frametime(self));
  }
  
  camera_update(self->camera);
  // call ruby update callback 
  mval = mrb_float_value(state_ruby(self), state_frametime(self));
  rh_runtopfunction_console(state_console(self), state_ruby(self), 
                                 "on_update", 1, &mval);
  
}


/** Polls the state's event queue, and gets the next event and stores it in
* event. if it is available. Returns true if there is an event of false if
* not.
*/
int state_poll(State * state,  ALLEGRO_EVENT * event) {
  return al_get_next_event(state->queue, event);
}  

/** Polls the state's event queue, and gets the next event and returns it. 
* returns nul if out of memory or no event was available.
* You must free the result of this function with event_free
*/
ALLEGRO_EVENT * state_pollnew(State * state) {
  ALLEGRO_EVENT event, * result;
  
  if(state_poll(state, &event)) {
    result = event_alloc();
    if(!result) return NULL;
    (*result) = event; // copy data
    return result;     // return pointer
  }
  return NULL;
}


/** Return the state's default font. */
ALLEGRO_FONT * state_font(State * state) {
  if(!state) return NULL;
  return state->font;
}


/** Call this every frame to update the FPS and frames value */
void state_frames_update(State * state) {
  double now = al_get_time();
  // tthis algoritm gives a
  state->frames++;
  if(state->frames < 0)  { // oops, overflow, so handle it.
    state->frames  = 0;
    state->fps     = 0;
    state->fpstime = now;
  } else { 
    state->fps     = ((double)state->frames) / (now - state->fpstime);
  }   
}

/** Returns the amount of frames rendered during this second. */
int state_frames(State * state) {
  return state->frames;
}

/** Returns the FPS value. */
double state_fps(State * state) {
  return state->fps;
}

/** Returns the Frame time value. */
double state_frametime(State * state) {
  if( state->fps < 20.0) return  1.0 / 20.0; 
  return 1.0 / state->fps;
}



/** Returns the camera of the state. */
Camera * state_camera(State * state) {
  if(!state) return NULL;
  return state->camera;
}





/* Core functionality of Eruta, implemented on the state. */

/* Ideas about starting the game and progressing
* Normally, the game will have different modes. The game will start in 
* intro mode, which automatically changes into main menu mode. 
* From main menu mode a game may be loaded or a new game may be started. 
* 
* When the game starts, the generic startup and settings scripts must be loaded.
* They influence the design of the main menu. 
*  
* To begin, we must implement the new game start. When a new game is started,
* the game start script is loaded. Then, unless instructed differently, 
* map number 0001 is loaded. As always, when a map is loaded, 
* the map's corresponding script is loaded. The game has an area of "savable"
* data which is initialized either by loading the game or by the new game script.   
*  
* Before the game is saved, the save game script is loaded. Likewise before 
* the game is loaded the load game script is loaded. 
* 
* Once the game is running, the main game script is loaded and call-backs 
* are used to communicate with it. This is the mechanism that allows part of the game 
* to be implemented in script. 
* 
* Needed scripts so far: start.mrb, newgame.mrb, loadgame.mrb, savegame.mrb, 
* main.mrb, and one map_0001.mrb, etc for every map. Other scripts can be loaded by 
* these scripts for modulization, but only from the script directory.  
*  
*/



/* Preloads a tile map from the given vpath. Returns the loaded map, or 
 NULL if not loaded. */
Tilemap * state_preloadmap_vpath(State * state, const char * vpath) {
  
  return NULL;
}

/* Preloads a tile map with the given map number. Returns the loaded map, or 
 NULL if not loaded. */
Tilemap * state_preloadmap_index(State * state, int index) {
  return NULL;
}

/* Tints a whole layer of a given sprite */
int state_sprite_tintlayer
(State * state, int sprite_index, int layer_index, int r, int g, int b, int a) {
  Sprite * sprite = state_sprite(state, sprite_index);
  Color color   = al_map_rgba(r, g, b, a);
  if (!sprite) { return -1; }
  sprite_tintlayer(sprite, layer_index, color);
  return 0;
}



#ifdef _COMMENT

/* Returns the active mode of the state if any. */
Mode * state_mode(State * state) {
  if(!state) return NULL;
  return (state->mode);
}

/* Sets the active mode of the state to the one with the given index.
* returns the mode set or NULL if not set.
*/
Mode * state_modeindex_(State * state, int index) {
  State * old, * next;
  if(!state) return NULL;
  if((index<0) || (index > ERUTA_MODE_MAX)) return NULL;
  old  = state->mode;
  next = state->modes[index];
  // try to leave old mode 
  if(!mode_leave(old)) return NULL;
  // couldn't enter next mode, return to previous mode.
  if(!mode_enter(next)) {
    mode_enter(old);
    return NULL;
  }
  // If we get here the mode was entered correctly.
  state->mode = next;  
  return state->mode;
}

#endif
