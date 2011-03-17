#include <stdlib.h>

#include "gari.h"
#define GARI_INTERN_ONLY
#include "gari_intern.h"
#include <time.h>



struct GariImage_ {
  SDL_Surface screen;
};

struct GariScreen_ {
  GariImage image;
};


struct GariJoystick_ {
  int             index;
  SDL_Joystick *  handle;
};

/** Returns the amount of joysticks connected to this system. */
int gari_joy_amount() {
  return SDL_NumJoysticks();
}


const char * gari_joy_nameindex(int index) {  
  return SDL_JoystickName(index);
} 
 
const char * gari_joy_name(GariJoystick * joy) {
  if(!joy) return NULL;
  return gari_joy_nameindex(joy->index);
} 

int gari_joy_axes(GariJoystick * joy) {
  if(!joy) return -1;
  return SDL_JoystickNumAxes(joy->handle);
}

int gari_joy_buttons(GariJoystick * joy) {
  if(!joy) return -1;
  return SDL_JoystickNumAxes(joy->handle);
}

int gari_joy_balls(GariJoystick * joy) {
  if(!joy) return -1;
  return SDL_JoystickNumBalls(joy->handle);
}

int gari_joy_index(GariJoystick * joy) {
  if(!joy) return -1;
  return (joy->index);
}


/** describes a joystick. */
void gari_joy_describe(GariJoystick * joy) { 
  if(!joy) { return;}  
  fprintf(stderr, "Joystick Name: %s ", gari_joy_name(joy));
  fprintf(stderr, "Axes: %d ", gari_joy_axes(joy));
  fprintf(stderr, "Buttons: %d ", gari_joy_buttons(joy));
  fprintf(stderr, "Balls: %d \n", gari_joy_balls(joy));
}



GariJoystick * gari_joy_alloc() {
  return GARI_MALLOC(sizeof(GariJoystick));
}

GariJoystick * gari_joy_open(int index) {
  GariJoystick * joy = gari_joy_alloc();
  if(!joy) return NULL;  
  joy->index  = index; 
  joy->handle = SDL_JoystickOpen(index);
  if(joy->handle) return joy;
  gari_joy_free(joy);
  return NULL;
} 

GariJoystick * gari_joy_close(GariJoystick * stick) {
  if(!stick) return NULL;
  SDL_JoystickClose(stick->handle);
  stick->handle = NULL;
  stick->index  = -1;
  return stick;
} 

void gari_joy_free(GariJoystick * stick) {
  GARI_FREE(gari_joy_close(stick));   
}



struct GariGame_ {
  GariScreen * screen;
  int          error;
  char *       message;
  /* For FPS calculation. */  
  uint32_t     frames;  
  uint32_t     framestart;
  uint32_t     framestop;
  uint32_t     delta;
  GariJoystick **joysticks;
};


GariGame * gari_game_make() {
  GariGame * game = GARI_MALLOC(sizeof(GariGame));
  if(gari_game_init(game)) { return game; }
  gari_game_free(game);
  return NULL;  
}


// Opens all available joysticks.
GariGame * gari_game_openjoysticks(GariGame * gari) {
  int index;
  int amount = gari_joy_amount();
  if(amount<1) return NULL;
  gari->joysticks = GARI_MALLOC(sizeof(GariJoystick *) * amount);
  for(index = 0; index < amount; index ++) {
    gari->joysticks[index] = gari_joy_open(index);
    // gari_joy_describe(gari->joysticks[index]);
  }
  return gari;
}

// Closes and deallocates all available joysticks.
GariGame * gari_game_closejoysticks(GariGame * gari) {
  int index;
  int amount = gari_joy_amount();
  if(amount<1) return NULL;
  if(!(gari->joysticks)) return NULL;
  for(index = 0; index < amount; index ++) {
    gari_joy_free(gari->joysticks[index]);        
  }
  GARI_FREE(gari->joysticks);
  gari->joysticks 	= NULL;
  return gari;
}


/** Returns how many joysticks are available. */
int gari_game_joysticks(GariGame * game) {
  return gari_joy_amount();
}

/** Returns the n-th joystick of the game. */
GariJoystick * gari_game_joystick(GariGame * game, int index) {
  if (index > gari_joy_amount()) return NULL;
  return game->joysticks[index];
}

/** Sets the keyboard repeat delay and interval in ms, 
*   or disable with delay 0. */
GariGame * gari_game_keyrepeat(GariGame * game, int delay, int interval) { 
  SDL_EnableKeyRepeat(delay, interval);
}


/** Initializes a gari game, opeing all joysticks, etc. */
GariGame * gari_game_init(GariGame * game) {
  if (!game) { return NULL; }
  gari_game_resetframes(game);
  game->error  		= 0;
  game->screen 		= NULL;
  game->joysticks 	= NULL;
  if(SDL_Init(SDL_INIT_EVERYTHING)) {
    fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
    game->error = 1;
    return NULL;
  }
  // also initialize TTF
  if(TTF_Init()==-1) {
    fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
    return NULL;
  }
  // also enable unicode events.
  SDL_EnableUNICODE(1);
  
  // also open all joysticks.
  gari_game_openjoysticks(game);
  
  return game;
} 

GariGame * gari_game_done(GariGame * game) {
  if (!game) { return NULL; } 
  gari_game_closejoysticks(game);
  // also close all joysticks.
  game->screen = NULL;
  // if (TTF_WasInit()) TTF_Quit();
  // don't quit ttf, so fonts can still be deallocated
  SDL_Quit();
  return game;
}

void gari_game_free(GariGame * game) {  
  gari_free(gari_game_done(game));  
}

GariScreen * gari_game_screen(GariGame * game) {
  return game->screen;
}

// Converts between GariScreen and SDL_Surface
SDL_Surface * gari_screen_surface(GariScreen * screen) {
  return  (SDL_Surface *)(screen);
}

// Converts between SDL_Surface and GariScreen
GariScreen * gari_surface_screen(SDL_Surface * surf) {
  return  (GariScreen *)(surf);
}

// Checks if a screen is the fullscreen screen or not
int gari_screen_fullscreen(GariScreen * screen) {
  if(!screen) return 0;
  return gari_screen_surface(screen)->flags & SDL_FULLSCREEN;
}

// Checks if the main game  window is in fullscreen mode. 
int gari_game_fullscreen(GariGame * game) {
  return gari_screen_fullscreen(gari_game_screen(game));
}

// Opens the game screen with the given parameters.
GariScreen * gari_game_openscreendepth(GariGame * game, int wide, int high, int fullscreen, int depth) {
  int new_depth = 0;
  SDL_Surface * screen = NULL;
  Uint32  flags = 0;  
  if (game->screen) { return game->screen; }
  if(depth < 1)     { depth = GARI_DEFAULT_DEPTH; } 
  flags             = SDL_HWSURFACE;
  if (fullscreen)   { flags     |= SDL_FULLSCREEN; }
  new_depth  = SDL_VideoModeOK(wide, high, depth, flags);
  if (!new_depth )  { 
    flags           |= SDL_ANYFORMAT;
    new_depth        = SDL_VideoModeOK(wide, high, GARI_DEFAULT_DEPTH, flags);
    if(!new_depth) {
      fprintf(stderr, "\nUnable to open screen: %dx%d@%d : %s\n", wide, high, new_depth, SDL_GetError());
      return NULL;
    }
  }  
  screen = SDL_SetVideoMode(wide, high, new_depth, flags);
  if (!screen) {
      fprintf(stderr, "\nUnable to open screen: %dx%d@%d : %s\n", wide, high, new_depth, SDL_GetError());
      return NULL;
  }
  game->screen = (GariScreen *) screen;
  return game->screen;
}


GariScreen * gari_game_openscreen(GariGame * game, int wide, int high, 
                                  int fullscreen) {
  return gari_game_openscreendepth(game, wide, high, 
            fullscreen, GARI_DEFAULT_DEPTH);  
}


/**
/ Tries to change screen resolution or flags. 
*/
GariScreen * gari_game_changescreen(GariGame * game, int w, int h, int depth, uint32_t flags) {
  GariScreen * screen = gari_game_screen(game);
  SDL_Surface * surf  = gari_screen_surface(screen);
  SDL_Surface * newsurf;
  int oldflags = surf->flags;
  if (!screen) { return NULL; }  
  newsurf = SDL_SetVideoMode(w, h, depth, flags);
  if(!newsurf) newsurf = SDL_SetVideoMode(0, 0, 0, oldflags);
  if(!newsurf) { return NULL; }  
  game->screen = gari_surface_screen(newsurf);
  return game->screen;
} 

GariScreen * gari_game_fullscreen_set(GariGame * game) {
  GariScreen * screen = gari_game_screen(game);
  SDL_Surface * surf  = gari_screen_surface(screen); 
  return gari_game_changescreen(game, 0 ,0, 0, surf->flags | SDL_FULLSCREEN);
}


GariScreen * gari_game_fullscreen_unset(GariGame * game) {
  GariScreen * screen = gari_game_screen(game);
  SDL_Surface * surf  = gari_screen_surface(screen); 
  return gari_game_changescreen(game, 0 ,0, 0, surf->flags & (~SDL_FULLSCREEN));
}


/** Can be used to set or unset fullscreen after opening the screen. */
GariScreen * gari_game_fullscreen_(GariGame * game, int fullscreen) {    
  if (fullscreen && !(gari_game_fullscreen(game))) {
    return gari_game_fullscreen_set(game);
  } else if (!fullscreen && (gari_game_fullscreen(game))) {
    return gari_game_fullscreen_unset(game);
  }
  return gari_game_screen(game);
}


/** Advances the frame counter of the game. */
GariGame * gari_game_nextframe(GariGame * game) {
  game->frames++;
  return game;
}

/** Updates the game screen, FPS, etc. */
GariGame * gari_game_update(GariGame * game) {
  if(game->screen) SDL_Flip((SDL_Surface *)game->screen);
  gari_game_nextframe(game);
  return game;
}

/** Sets the frames of the game back to 0 and starts the FPS counter.*/
GariGame * gari_game_resetframes(GariGame * game) {  
  game->frames = 0;
  gari_game_startfps(game);
  return game;
}

/** Gets the total amount of frames displayed since the start of the game, 
   or since gari_game_resetframes was called. */
uint32_t gari_game_frames(GariGame * game) {
  return game->frames;
}

/** Starts FPS counter. */
GariGame * gari_game_startfps(GariGame * game) {
  game->framestart = SDL_GetTicks();   
  return game;
}

/** Retuns calculated fps after calling startfps . */
double gari_game_fps(GariGame * game) {
  double dd, df;
  game->framestop   = SDL_GetTicks();
  game->delta       = game->framestop - game->framestart;
  df                = (double)(game->frames);
  dd                = (double)(game->delta);
  return (1000.0 * df) / dd ;
}

void gari_game_report(GariGame * game)  {
  double fps         = gari_game_fps(game);
  fprintf(stderr, "FPS %d frames / %d ms: %lf fps.\n", game->frames, game->delta, fps);
}

/** Quickly fills the image or screen with the given dye */
void gari_image_filldye(GariImage * image,  GariDye dye) {
  SDL_FillRect((SDL_Surface *) image, NULL, (Uint32) dye);
}

/** Quickly draws a rectangle with the given dye, without blending. */
void gari_image_fillrectdye(GariImage * image, int x, int y, 
                          int w, int h, GariDye dye) {
   SDL_Rect rect = { x, y, w, h };
   SDL_FillRect((SDL_Surface *) image, &rect, (Uint32) dye);
}

/** Quickly fills the image with the given color. */
void gari_image_fill(GariImage * image,  GariColor color) {
  GariDye dye = gari_color_dye(color, image);
  gari_image_filldye(image, dye);  
}

/** Quickly draws a rectangle with the given dye, without blending. */
void gari_image_fillrect(GariImage * image, int x, int y, 
                          int w, int h, GariColor color) {
   GariDye dye = gari_color_dye(color, image);
   gari_image_fillrectdye(image, x, y, w, h, dye);
}


 

/** Returns the drawable image of the screen. */
GariImage * gari_screen_image(GariScreen * screen) {
  return &(screen->image);
}


/** Initializes the random number generator. */
void gari_random_init() {
  srandom(((long)clock()));
}

/** Gets a random number between min and max. */
long gari_random(long min, long max) {
  return (random() % (max - min)) + min;
}


/** Hides or shows the default mouse cursor over the screen. Returns the previous state of the mouse cursor.  */
int gari_screen_showcursor_(GariScreen * screen, int show) {
  int toggle = show ? SDL_ENABLE : SDL_DISABLE;
  return SDL_ShowCursor(toggle) == SDL_ENABLE;
}  

/** Returns true if the cursor is shown, false if not. */
int gari_screen_showcursor(GariScreen * screen) {  
  return SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE;
}  

