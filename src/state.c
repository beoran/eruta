#include "eruta.h"
#include "mem.h"
#include "state.h"
#include "camera.h"
#include "sound.h"
#include "tilemap.h"
#include "dynar.h"
#include "mode.h"
#include "fifi.h"
#include "lh.h"
#include "rh.h"
#include "toruby.h"
#include "event.h"
#include "widget.h"


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
  Tilemap             * nowmap;  // active tile map
  Tilemap             * loadmap; // new tile map that's being loaded.
  Mode                * mode; // active mode
  Dynar               * modes;
  Camera              * camera;
  Ruby                * ruby;
  Console             * console; 
  // The ruby and error message GUI console.
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
Tilemap * state_loadmap(State * state) {
  if(!state) return NULL;
  return state->loadmap;
}


/** Allocates a state struct */
State * state_alloc() {
  return STRUCT_ALLOC(State);
}

/** Frees a state struct */
void state_free(State * self) {
  
  dynar_free(self->modes);
  rh_free(self->ruby);
  console_free(self->console);
  self->console = NULL; // disable console imediately.
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
Console * state_console(State * state) {
  return state->console;
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
  
  // initialize the file finder, so we can start to loaded
  if(!fifi_init()) { 
    return state_errmsg_(self, "Could not find data folder!\n");
  }

/*
  self->font = fifi_loadfont("fixed_font.tga", 0, 0);
  if (!self->font) {
    return state_errmsg_(self, "Error loading data/font/fixed_font.tga");
  }
*/ // Tuffy.ttf
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
  self->fps     = 0.0;
  self->fpstime = al_get_time();
  self->frames  = 0;  
  self->loadmap = NULL;
  self->nowmap  = NULL;
  
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
  {
    Style style = { color_rgb(255,255,255), color_rgba(64,0,0, 191), 
                    self->font, NULL};
    Bounds bounds = { {0, 0,} , {640, 480} }; 
    self->console = console_new(bounds, style);
    if(!self->console) {
      return state_errmsg_(self, "Out of memory when allocating console.");
    }
  }
  console_puts(self->console, "Console started ok!");
  // set up ruby callback for console commands 
  console_command_(self->console, 
                   (ConsoleCommand *)rh_dostring_console, self->ruby);
    
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