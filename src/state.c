#include "eruta.h"
#include "mem.h"
#include "state.h"
#include "camera.h"
#include "sound.h"
#include "tilemap.h"
#include "tileio.h"
#include "dynar.h"
#include "draw.h"
#include "mode.h"
#include "fifi.h"
#include "rh.h"
#include "toruby.h"
#include "event.h"
#include "widget.h"
#include "area.h"
#include "thing.h"
#include "sprite.h"
#include "scegra.h"
#include "monolog.h"
#include <store.h>

/* The data struct contains all global state and other data of the application.
*/
struct State_ {
  /* State flags. */
  BOOL                  busy;
  BOOL                  fullscreen;
  BOOL                  audio;
  
  /* Graphics mode. XXX: I think???? :P */
  int32_t               modeno;
  
  /* 
   There are two fonts for now, ttf and font. Font is a plain font
   for emergency use, ttf is the normal font for the console.
  */
  ALLEGRO_FONT        * font;
  ALLEGRO_FONT        * ttf;
  /* Some default colors */
  ALLEGRO_COLOR         colors[STATE_COLORS];
  
  
  
  /* Display */
  ALLEGRO_DISPLAY     * display;
  
  ALLEGRO_EVENT_QUEUE * queue;
  char                * errmsg;
  
  
  /* FPS handling. */
  double                fpsnow, fpstime, fps;
  int                   frames;
  
  /* Background image that can be set behind the tile map. */
  ALLEGRO_BITMAP      * background_image;
  
  /* Background color, in case of no tile map */
  ALLEGRO_COLOR         background_color;
  
  
  /* Active tile map, linked from and loaded through a Store ID. */
  Tilemap             * active_map; 
  int                   active_map_id;
  
  /* Does the area needs to be displayed or not. */
  int                   show_area;
  
  /* Does the graph needs to be displayed or not */
  int                   show_graph;
  
  /* Does the FPS counter needs to be displayed or not? */
  int                   show_fps;
  
  /* Logical and physical game objects. This one is always active, regardless of the tile map. */
  Area                * area;
  // View camera for the area, tile map and particle engine. 
  Camera              * camera;
  
  /* Mode is removed, this will be handled on the scripting side. */
    
  /* Sprite subsystem */
  SpriteList          * sprites;
  /* Ruby subsystem  */
  Ruby                * ruby;  
  /* 
   The ruby and error message GUI console.
   Implemented in C so it's usable even if there are script bugs.   
  */
  BBConsole           * console;   
  /* The current actor, controlled by the player. */
  Thing               * actor;

  
};






/** State variable.   */
static State * global_state_ = NULL;

/** Various loggers. One for stdout if not on windows,
 * one to a file and one to the BBConsole */
int state_console_logf(char * file, int line, char * level,
                       void * data, char * format, va_list args) {
  BBConsole * console = data;
  if (console) {
   return bbconsole_vprintf(console, format, args);
  }
  return -1;
}



DEFINE_STDERR_LOGGER(state_stderr_logger);
DEFINE_FILE_LOGGER(state_file_logger);
DEFINE_LOGGER(state_console_logger, state_console_logf, NULL);
 

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

/** Returns the state's active tile map. */
Tilemap * state_active_map(State * state) {
  if(!state) return NULL;
  return state->active_map;
}


/** Sets the state's active tile map. Also sets it in the state's area, and
  disables all previous camera panners and lockins, and sets up a basic lockin on 
  map layer 0 if the map is not NULL. */
Tilemap * state_active_map_(State * state, Tilemap * map) {
  if(!state) return NULL;
  
  if (state->area) {
    area_tilemap_(state->area, map);
  }      
  
  if (state->camera) { 
    camera_freepanners(state->camera);
    camera_freelockins(state->camera);
  }
  
  state->active_map = map;
  
  if(state->active_map) {
    tilemap_layer_lockin(state->active_map, 0, state->camera);
  }
  
  return state->active_map;
}

/** Gets the store index of the state's active tile map, or -1 if none set. */
int state_active_map_id(State * state) {
  if(!state) return -1;  
  return state->active_map_id;
}


/** Sets the state's active tile from the Store system index. 
 * Returns negative on failure, or the index set. Disables the active tile map if index is < 0 */
int state_active_map_id_(State * state, int index) {
  Tilemap * map;
  if (!state) return -1;  
  if (index < 0) {
    state->active_map_id = -1;
    state_active_map_(state, NULL);
    return -1;
  }
  map = store_get_other(index, RESOR_TILEMAP);
  if (!map) {
    // refuse to load nonexisting map
    return -2;
  }
  state_active_map_(state, map);
  return -1;
}



/** Returns the state's area. */
Area * state_area(State * state) {
  if (!state) return NULL;
  return state->area;
}

/** Return's the state's sprite list */
SpriteList * state_sprites(State * state) {
  if (!state) return NULL;
  return state->sprites;
}


/** Allocates a state struct */
State * state_alloc() {
  return STRUCT_ALLOC(State);
}

/** Frees a state struct. */
void state_free(State * self) {
  
  /* Shut down audio */
  audio_done();
  
  spritelist_free(self->sprites);
  self->sprites = NULL;
  area_free(self->area);
  self->area    = NULL;
  /* Disable the active tile map */
  state_active_map_id_(self, -1);
  
  rh_free(self->ruby);
  bbconsole_free((BBWidget *)self->console, NULL);
  self->console = NULL; /* disable console immediately. */
  /* Deallocate stored objects. */
  store_done();
 
  // font_free(self->font);
  al_destroy_display(self->display);
  camera_free(self->camera);

  al_uninstall_system();
  
  /* End logging. */
  monolog_done();  
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

/*  Gets a thing from the state's area. */
Thing * state_thing(State * state, int index) {
  Area * area = state_area(state);
  return area_thing(area, index);
}

/* Makes a new dynamic thing in the state's active area. */
Thing * state_newthing(State * state, int index, int kind, 
                        int x, int y, int z, int w, int h) {
  Area * area = state_area(state);
  return area_newdynamic(area, index, kind, x, y, z, w, h);
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
 
/* Looks up the thing by index and let the state's camera track it. 
 * If index is negative, stop tracking in stead.
 */
int state_camera_track_(State * state, int thing_index) {
  Thing * thing;
  if (thing_index < 0) {
    camera_track_(state_camera(state), NULL);
    return -2;
  }  
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

/* Lock in the state's camera to the current active tile map's given layer. 
 * Returns negative if no tile map is currently active. 
 */
int state_lockin_maplayer(State * state, int layer) {  
  if(!state_active_map(state)) {
    return -1;
  } else { 
    tilemap_layer_lockin(state_active_map(state), layer, state_camera(state));
  }
  return 0; 
}


/* Loads a named tile map from the map folder. */
/* This function is obsolete. Load tile maps though script / store now. 
int state_loadtilemap_vpath(State * self, char * vpath) {
  TilemapLoadExtra extra;
  extra.area = self->area;
  self->loadingmap = fifi_load_vpath(tilemap_fifi_load, &extra, vpath);
  if(!self->loadingmap) return -1;
  tilemap_free(self->nowmap);
  self->nowmap = self->loadingmap;
  return 0;
}
*/

/* Sets the state current active actor to the thing with the given index.
 * Does not change if no such thing is found;
 */

/* Obsolete.  The scripts manage the active actor now. 
int state_actorindex_(State * self, int thing_index) {
  Thing * thing;
  thing = state_thing(self, thing_index);
  if(!thing) return -1;
  self->actor = thing;
  return thing_index;
}
*/

/* Returns the current active actor of the state. */
/* Obsolete.  The scripts manage the active actor now. 
Thing * state_actor(State * self) {
  if(!self) return NULL;
  return self->actor;
}
*/





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
  // initialize logging first
  if (!monolog_init()) {
    return state_errmsg_(self, "Could not init logging.");
  }

  // Initialize loggers
  monolog_add_logger(NULL, &state_stderr_logger);
  monolog_add_logger(fopen("eruta.log", "a"), &state_file_logger);
  // initialize log levels
  monolog_enable_level("error");
  monolog_enable_level("warning");
  monolog_enable_level("note");
  LOG_NOTE("Starting logging", "");
  
  
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

  /* Set up the audio system */
  self->audio = audio_init();
  if(!self->audio) {
    perror("Sound not started.");
  }

  // Use full screen mode if needed.
  if(self->fullscreen) { 
    flags = ALLEGRO_FULLSCREEN | ALLEGRO_GENERATE_EXPOSE_EVENTS;
  } else {
    /* flags = ALLEGRO_FULLSCREEN_WINDOW | ALLEGRO_GENERATE_EXPOSE_EVENTS; */
  }
  // flags |= ALLEGRO_OPENGL;
 
  al_set_new_display_flags(flags);
 /*  al_set_new_display_option(ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST); */
  // Create a window to display things on: 640x480 pixels.
  // self->display = al_create_display(1280, 960);
  self->display = al_create_display(SCREEN_W, SCREEN_H);
  if (!self->display) {
    return state_errmsg_(self, "Error creating display.\n");
  }
  
  
  
  // al_resize_display(self->display, SCREEN_W, SCREEN_H);
  
  // initialize the file finder, so we can start to load the data files
  if(!fifi_init()) { 
    return state_errmsg_(self, "Could not find data folder!\n");
  }
  
  // initialize the resource storage so we can store the data files
  if(!store_init()) { 
    return state_errmsg_(self, "Could not initialize data store.\n");
  }

  // Tuffy.ttf
  // "OpenBaskerville-0.0.53.otf"
  #define STATE_FONTNAME        "GranaPadano.ttf"
  #define STATE_FONT_INDEX      20000
  
  if(!store_load_ttf_font(STATE_FONT_INDEX, "font/" STATE_FONTNAME, -16, 0)) {
    return state_errmsg_(self, "Error loading " STATE_FONTNAME);
  }
  
  self->font = store_get_font(STATE_FONT_INDEX);
  
  // fifi_loadfont(STATE_FONTNAME, 16, 0);
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
  // set up fps counter. Start with assuming we have 60 fps. 
  self->fps        = 60.0;
  self->fpstime    = al_get_time();
  self->frames     = 60;    
  /* No active map yet. */
  state_active_map_id_(self, -1);
  
  /* Background color. */
  self->background_color = al_map_rgb(64,128,64);
  
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

  // set up logging to console
  monolog_add_logger(self->console, &state_console_logger);

  
  /* Initialize Area. */
  self->area = area_new();
  
  /* Initialize sprite list. */
  self->sprites = spritelist_new();
  
  /* Show all by default. */
  self->show_area = TRUE;
  self->show_fps  = TRUE;
  self->show_graph= TRUE;
  
  
  
  
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

/* Scales and moves the display to achieve resolution independence. */
void state_scale_display(State * self) {
   int real_w  = al_get_display_width(self->display);
   int real_h  = al_get_display_height(self->display);
   int scale_x = real_w / SCREEN_W;
   int scale_y = real_h / SCREEN_H;
   int scale   = (scale_x < scale_y) ? scale_x : scale_y;
   int offset_x= (real_w  - (SCREEN_W * scale)) / 2;
   int offset_y= (real_h  - (SCREEN_H * scale)) / 2;
   /*
   al_draw_textf(state_font(self), COLOR_WHITE,
                 100, 100, 0, "SCALE: w: %d, h: %d, sx: %d, sy: %d, s: %d, ox:%d, oy:%d",
                 real_w, real_h, scale_x, scale_y, scale, offset_x, offset_y);
    */
   ALLEGRO_TRANSFORM transform;
   al_identity_transform(&transform);
   /* Now draw black bars to cover the usused areas. */
   if (offset_y > 0) { 
    al_draw_filled_rectangle(-offset_x , -offset_y, real_w, 0, al_map_rgb(0,0,0));
    al_draw_filled_rectangle(-offset_x , SCREEN_H, real_w, SCREEN_H+offset_y, al_map_rgb(0,0,0));
   }
   if (offset_x > 0) { 
    al_draw_filled_rectangle(-offset_x , -offset_y, 0, real_h, al_map_rgb(0,0,0));
    al_draw_filled_rectangle(SCREEN_W  , -offset_y,  SCREEN_W + offset_x, real_h, al_map_rgb(0,0,0));
   }
   
   al_scale_transform(&transform, scale, scale);
   al_translate_transform(&transform, offset_x, offset_y);
   al_use_transform(&transform);
   /* al_set_clipping_rectangle(offset_x, offset_y, SCREEN_W, SCREEN_H); */
   
   
}


/* Draws all inside the state that needs to be drawn. */
void state_draw(State * self) {
    int layer;
    
    /* Draw background color if no map active. */
    if (!self->active_map) {
      al_clear_to_color(self->background_color);
    }    
    
    /* Draw the layers of the map and area interleaved. */
    for (layer = 0; layer < TILEMAP_PANES; layer++) {
      if (self->active_map) {
        /* Shadows should be drawn *before* the blends, otherwise both won't
         * look good when combined with each other. The problem with that is,
         * though that shadows are then not cast on the sprites.
         * Perhaps sprites will need separate shadows???
         */
        tilemap_draw_layer_tiles(self->active_map, self->camera, layer);
        tilemap_draw_layer_shadows(self->active_map, self->camera, layer);
        tilemap_draw_layer_blends(self->active_map, self->camera, layer);
      }
      if (self->area && self->show_area) {
        area_draw_layer(self->area, self->camera, layer);        
      }
    }
    
    /* Draw UI scene graph */
    if (self->show_graph) { 
      scegra_draw();
    }
    /* Draw the particles from the particle engine. */
    // alpsshower_draw(&shower, state_camera(state));
    
    
    /* Draw fps if needed.  */
    if (self->show_fps) { 
      al_draw_textf(state_font(self), COLOR_WHITE,
                        10, 10, 0, "FPS: %.0f", state_fps(self));
    } 
    
    /* Draw the console (will autohide if not active). */
    bbwidget_draw((BBWidget *)state_console(self));
    state_scale_display(self);
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
  
  if (self->active_map) { 
    tilemap_update(self->active_map, state_frametime(self));
  }
  
  if (self->area) {
      area_update(self->area, state_frametime(self));
  }
  
  camera_update(self->camera);
  // call ruby update callback 
  mval = mrb_float_value(state_ruby(self), state_frametime(self));
  rh_runtopfunction_console(state_console(self), state_ruby(self), 
                                 "on_update", 1, &mval);
  // Update the scene graph
  scegra_update(state_frametime(self));
  
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
  state->frames++;

  if((now - state->fpstime) > 1.0) {
    double realfps;
     /* Measure only last second of frames, which means FPS gets updated every second or so. */
     realfps = ((double)state->frames) / (now - state->fpstime);
     /* Display and use a rounded value for FPS, the number after the comma is normally due to jitter anyway. */
     state->fps = floor(realfps + 0.5);

     /* A little trick, to prefent jerkyness, 
      * keep half the frames; and half the time  */
     state->frames  = state->frames / 2;
     state->fpstime = now - 0.5;
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

/* Get display state */
int global_state_show_fps() {
  State * state = state_get();
  if (!state) return FALSE;
  return state->show_fps; 
}

/* Set display state */
int global_state_show_fps_(int show) {
  State * state = state_get();
  if (!state) return FALSE;
  return state->show_fps = show;
}

/* Get display state */
int global_state_show_graph() {
  State * state = state_get();
  if (!state) return FALSE;
  return state->show_graph; 
}

/* Set display state */
int global_state_show_graph_(int show) {
  State * state = state_get();
  if (!state) return FALSE;
  return state->show_graph = show;
}

/* Get display state */
int global_state_show_area() {
  State * state = state_get();
  if (!state) return FALSE;
  return state->show_area; 
}

/* Set display state */
int global_state_show_area_(int show) {
  State * state = state_get();
  if (!state) return FALSE;
  return state->show_area = show;
}

/* Get display state of physics */
int global_state_show_physics() {
  State * state = state_get();
  if (!state) return FALSE;
  return area_draw_physics(state->area);
}

/* Set display state of physics */
int global_state_show_physics_(int show) {
  State * state = state_get();
  if (!state) return FALSE;
  area_draw_physics_(state->area, show);
  return show;
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

/* Tints a whole layer of a given sprite. */
int state_sprite_tintlayer
(State * state, int sprite_index, int layer_index, int r, int g, int b, int a) {
  Sprite * sprite = state_sprite(state, sprite_index);
  Color color   = al_map_rgba(r, g, b, a);
  if (!sprite) { return -1; }
  sprite_tintlayer(sprite, layer_index, color);
  return 0;
}


/* Transforms a mask color of an image in storage into an alpha. */
int state_image_mask_to_alpha(State * state, int store_index, int r, int g, int b) {
  Image * image = store_get_bitmap(store_index);
  Color color   = al_map_rgb(r, g, b);
  if (!image) return -1;  
  al_convert_mask_to_alpha(image, color);
  return store_index;
}



/* Transforms an image in storage where the average is assigned to the alpha value. */
int state_image_average_to_alpha(State * state, int store_index, int r, int g, int b) {
  Image * image = store_get_bitmap(store_index);
  Color color   = al_map_rgb(r, g, b);
  if (!image) return -1;  
  draw_convert_average_to_alpha(image, color);
  return store_index;
}

/* Returns the first unused thing ID that is greater than minimum. */
int state_get_unused_thing_id(int minimum) {
  return area_get_unused_thing_id(state_area(state_get()), minimum);
}

/* Returns the first unused sprite ID that is greater than minimum. */
int state_get_unused_sprite_id(int minimum) {
  return spritelist_get_unused_sprite_id(state_sprites(state_get()), minimum);
}

