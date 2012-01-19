#include <stdlib.h>

#include "gy.h"
#define GY_INTERN_ONLY
#include "gy_intern.h"
#include <time.h>


struct GyImage_ {
  SDL_Surface screen;
};

struct GyScreen_ {
  GyImage image;
};


struct GyJoystick_ {
  int             index;
  SDL_Joystick *  handle;
};

/** Returns the amount of joysticks connected to this system. */
int gyjoy_amount() {
  return SDL_NumJoysticks();
}


const char * gyjoy_nameindex(int index) {  
  return SDL_JoystickName(index);
} 
 
const char * gyjoy_name(GyJoystick * joy) {
  if(!joy) return NULL;
  return gyjoy_nameindex(joy->index);
} 

int gyjoy_axes(GyJoystick * joy) {
  if(!joy) return -1;
  return SDL_JoystickNumAxes(joy->handle);
}

int gyjoy_buttons(GyJoystick * joy) {
  if(!joy) return -1;
  return SDL_JoystickNumAxes(joy->handle);
}

int gyjoy_balls(GyJoystick * joy) {
  if(!joy) return -1;
  return SDL_JoystickNumBalls(joy->handle);
}

int gyjoy_index(GyJoystick * joy) {
  if(!joy) return -1;
  return (joy->index);
}


/** describes a joystick. */
void gyjoy_describe(GyJoystick * joy) { 
  if(!joy) { return;}  
  fprintf(stderr, "Joystick Name: %s ", gyjoy_name(joy));
  fprintf(stderr, "Axes: %d ", gyjoy_axes(joy));
  fprintf(stderr, "Buttons: %d ", gyjoy_buttons(joy));
  fprintf(stderr, "Balls: %d \n", gyjoy_balls(joy));
}



GyJoystick * gyjoy_alloc() {
  return GY_MALLOC(sizeof(GyJoystick));
}

GyJoystick * gyjoy_open(int index) {
  GyJoystick * joy = gyjoy_alloc();
  if(!joy) return NULL;  
  joy->index  = index; 
  joy->handle = SDL_JoystickOpen(index);
  if(joy->handle) return joy;
  gyjoy_free(joy);
  return NULL;
} 

GyJoystick * gyjoy_close(GyJoystick * stick) {
  if(!stick) return NULL;
  SDL_JoystickClose(stick->handle);
  stick->handle = NULL;
  stick->index  = -1;
  return stick;
} 

void gyjoy_free(GyJoystick * stick) {
  GY_FREE(gyjoy_close(stick));
}


/* Allow joysticks to generate events or not.
 Call this  before calling openjoysticks */
int gyjoy_events_(int enable) {
  int state = enable ? SDL_ENABLE : SDL_IGNORE;
  return SDL_JoystickEventState(state);
}

// Opens all available joysticks.
GyJoystick ** gyjoy_openall() {
  GyJoystick ** result;
  int index;
  int amount  = gyjoy_amount();
  if(amount<1) return NULL;
  result      = calloc(sizeof(GyJoystick *), amount);
  if(!result) return NULL;
  for(index   = 0; index < amount; index ++) {
    result[index] = gyjoy_open(index);
  }
  return result;
}

/** Closes and deallocates all available joysticks in joysticks. Returns NULL. */
GyJoystick ** gyjoy_closeall(GyJoystick ** joysticks) {
  
  int index;
  if(!joysticks) return NULL;
  int amount = gyjoy_amount();
  if(amount<1) return NULL;
  if(!(joysticks)) return NULL;
  for(index = 0; index < amount; index ++) {
    gyjoy_free(joysticks[index]);
  }
  free(joysticks);
  joysticks = NULL;
  return NULL;
}


/** Sets the keyboard repeat delay and interval in ms, 
*   or disable with delay 0. */
int gy_keyrepeat(int delay, int interval) { 
  return SDL_EnableKeyRepeat(delay, interval);
}

/** Shuts down SDL and it's accessory libraries. */
void gy_quit(void) {
  if (TTF_WasInit()) TTF_Quit();
  // ?don't quit ttf, so fonts can still be deallocated???
  SDL_Quit();
}

/** Initializes SDL and it's accesory libraries. */
int gy_init_everything() {
  if(SDL_Init(SDL_INIT_EVERYTHING)) {
    fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
    return FALSE;
  }
  // also initialize TTF
  if(TTF_Init()==-1) {
    fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
    return FALSE;
  }
  // Also enable unicode events.
  SDL_EnableUNICODE(1);
  // enable joystick events if any joystics are present 
  if (gyjoy_amount() > 0) {
    gyjoy_events_(TRUE);
  }
  return TRUE;
}



struct GyGame_ {
  GyScreen * screen;
  int          error;
  char *       message;
  /* For FPS calculation. */  
  uint32_t     frames;  
  uint32_t     framestart;
  uint32_t     framestop;
  uint32_t     delta;
  GyJoystick **joysticks;
};


GyGame * gygame_make() {
  GyGame * game = GY_MALLOC(sizeof(GyGame));
  if(gygame_init(game)) { return game; }
  gygame_free(game);
  return NULL;
}

/* Allow joysticks to generate events or not.
 Call this  before calling openjoysticks */
int gygame_joystickevents_(GyGame * gy, int enable) {
  int state = enable ? SDL_ENABLE : SDL_IGNORE;
  return SDL_JoystickEventState(state);
}


// Opens all available joysticks.
GyGame * gygame_openjoysticks(GyGame * gy) {
  int index;
  int amount = gyjoy_amount();
  if(amount<1) return NULL;
  gy->joysticks = GY_MALLOC(sizeof(GyJoystick *) * amount);
  for(index = 0; index < amount; index ++) {
    gy->joysticks[index] = gyjoy_open(index);
    // gyjoy_describe(gy->joysticks[index]);
  }
  return gy;
}

// Closes and deallocates all available joysticks.
GyGame * gygame_closejoysticks(GyGame * gy) {
  int index;
  int amount = gyjoy_amount();
  if(amount<1) return NULL;
  if(!(gy->joysticks)) return NULL;
  for(index = 0; index < amount; index ++) {
    gyjoy_free(gy->joysticks[index]);
  }
  GY_FREE(gy->joysticks);
  gy->joysticks = NULL;
  return gy;
}


/** Returns how many joysticks are available. */
int gygame_joysticks(GyGame * game) {
  return gyjoy_amount();
}

/** Returns the n-th joystick of the game. */
GyJoystick * gygame_joystick(GyGame * game, int index) {
  if (index >= gyjoy_amount()) return NULL;
  return game->joysticks[index];
}

/** Sets the keyboard repeat delay and interval in ms, 
*   or disable with delay 0. */
GyGame * gygame_keyrepeat(GyGame * game, int delay, int interval) { 
  SDL_EnableKeyRepeat(delay, interval);
  return game;
}


/** Initializes a gy game. This opens SDL, SDL_ttf, all joysticks, etc. */
GyGame * gygame_init(GyGame * game) {
  if (!game) { return NULL; }
  gygame_resetframes(game);
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
  
  // enable joystick events if any joystics are present 
  if (gygame_joysticks(game) > 0) {
    gygame_joystickevents_(game, TRUE);
  }
  
  // also open all joysticks.
  gygame_openjoysticks(game);
  
  return game;
} 

GyGame * gygame_done(GyGame * game) {
  if (!game) { return NULL; } 
  gygame_closejoysticks(game);
  // also close all joysticks.
  game->screen = NULL;
  if (TTF_WasInit()) TTF_Quit();
  // ?don't quit ttf, so fonts can still be deallocated
  SDL_Quit();
  return game;
}

void gygame_free(GyGame * game) {  
  gyfree(gygame_done(game));  
}

GyScreen * gygame_screen(GyGame * game) {
  return game->screen;
}

// Converts between GyScreen and SDL_Surface
SDL_Surface * gyscreen_surface(GyScreen * screen) {
  return  (SDL_Surface *)(screen);
}

// Converts between SDL_Surface and GyScreen
GyScreen * gysurface_screen(SDL_Surface * surf) {
  return  (GyScreen *)(surf);
}

// Checks if a screen is the fullscreen screen or not
int gyscreen_fullscreen(GyScreen * screen) {
  if(!screen) return 0;
  return gyscreen_surface(screen)->flags & SDL_FULLSCREEN;
}

// Checks if the main game  window is in fullscreen mode. 
int gygame_fullscreen(GyGame * game) {
  return gyscreen_fullscreen(gygame_screen(game));
}

// Opens the game screen with the given parameters.
GyScreen * gygame_openscreendepth(GyGame * game, int wide, int high, int fullscreen, int depth) {
  int new_depth = 0;
  SDL_Surface * screen = NULL;
  Uint32  flags = 0;  
  if (game->screen) { return game->screen; }
  if(depth < 1)     { depth = 0; } 
  flags             = SDL_HWSURFACE;
  if (fullscreen)   { flags     |= SDL_FULLSCREEN; }
  new_depth  = SDL_VideoModeOK(wide, high, depth, flags);
  if (!new_depth )  { 
    flags           |= SDL_ANYFORMAT;
    new_depth        = SDL_VideoModeOK(wide, high, 0, flags);
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
  game->screen = (GyScreen *) screen;
  return game->screen;
}


GyScreen * gygame_openscreen(GyGame * game, int wide, int high, 
                                  int fullscreen) {
  return gygame_openscreendepth(game, wide, high, 
            fullscreen, GY_DEFAULT_DEPTH);  
}


/**
/ Tries to change screen resolution or flags. 
*/
GyScreen * gygame_changescreen(GyGame * game, int w, int h, int depth, uint32_t flags) {
  GyScreen * screen = gygame_screen(game);
  SDL_Surface * surf  = gyscreen_surface(screen);
  SDL_Surface * newsurf;
  int oldflags = surf->flags;
  if (!screen) { return NULL; }  
  newsurf = SDL_SetVideoMode(w, h, depth, flags);
  if(!newsurf) newsurf = SDL_SetVideoMode(0, 0, 0, oldflags);
  if(!newsurf) { return NULL; }  
  game->screen = gysurface_screen(newsurf);
  return game->screen;
} 

GyScreen * gygame_fullscreen_set(GyGame * game) {
  GyScreen * screen = gygame_screen(game);
  SDL_Surface * surf  = gyscreen_surface(screen); 
  return gygame_changescreen(game, 0 ,0, 0, surf->flags | SDL_FULLSCREEN);
}


GyScreen * gygame_fullscreen_unset(GyGame * game) {
  GyScreen * screen = gygame_screen(game);
  SDL_Surface * surf  = gyscreen_surface(screen); 
  return gygame_changescreen(game, 0 ,0, 0, surf->flags & (~SDL_FULLSCREEN));
}


/** Can be used to set or unset fullscreen after opening the screen. */
GyScreen * gygame_fullscreen_(GyGame * game, int fullscreen) {    
  if (fullscreen && !(gygame_fullscreen(game))) {
    return gygame_fullscreen_set(game);
  } else if (!fullscreen && (gygame_fullscreen(game))) {
    return gygame_fullscreen_unset(game);
  }
  return gygame_screen(game);
}


/** Advances the frame counter of the game. */
GyGame * gygame_nextframe(GyGame * game) {
  game->frames++;
  return game;
}

/** Updates the game screen, FPS, etc. */
GyGame * gygame_update(GyGame * game) {
  if(game->screen) SDL_Flip((SDL_Surface *)game->screen);
  gygame_nextframe(game);
  return game;
}

/** Sets the frames of the game back to 0 and starts the FPS counter.*/
GyGame * gygame_resetframes(GyGame * game) {  
  game->frames = 0;
  gygame_startfps(game);
  return game;
}

/** Gets the total amount of frames displayed since the start of the game, 
   or since gygame_resetframes was called. */
uint32_t gygame_frames(GyGame * game) {
  return game->frames;
}

uint32_t gygame_ticks(GyGame * game) {
  return (uint32_t) SDL_GetTicks();
}  

/** Starts FPS counter. */
GyGame * gygame_startfps(GyGame * game) {
  game->framestart = gygame_ticks(game);
  return game;
}

/** Retuns calculated fps after calling startfps . */
double gygame_fps(GyGame * game) {
  double dd, df;
  game->framestop   = SDL_GetTicks();
  game->delta       = game->framestop - game->framestart;
  df                = (double)(game->frames);
  dd                = (double)(game->delta);
  return (1000.0 * df) / dd ;
}

void gygame_report(GyGame * game)  {
  double fps         = gygame_fps(game);
  fprintf(stderr, "FPS %d frames / %d ms: %lf fps.\n", game->frames, game->delta, fps);
}

/** Quickly fills the image or screen with the given dye */
void gyimage_filldye(GyImage * image,  GyDye dye) {
  SDL_FillRect((SDL_Surface *) image, NULL, (Uint32) dye);
}

/** Quickly draws a rectangle with the given dye, without blending. */
void gyimage_fillrectdye(GyImage * image, int x, int y, 
                          int w, int h, GyDye dye) {
   SDL_Rect rect = { x, y, w, h };
   SDL_FillRect((SDL_Surface *) image, &rect, (Uint32) dye);
}

/** Quickly fills the image with the given color. */
void gyimage_fill(GyImage * image,  GyColor color) {
  GyDye dye = gycolor_dye(color, image);
  gyimage_filldye(image, dye);  
}

/** Quickly draws a rectangle with the given dye, without blending. */
void gyimage_fillrect(GyImage * image, int x, int y, 
                          int w, int h, GyColor color) {
   GyDye dye = gycolor_dye(color, image);
   gyimage_fillrectdye(image, x, y, w, h, dye);
}


 

/** Returns the drawable image of the screen. */
GyImage * gyscreen_image(GyScreen * screen) {
  return &(screen->image);
}


/** Initializes the random number generator. */
void gyrandom_init() {
  srandom(((long)time(NULL)));
}

/** Gets a random number between min and max. */
long gyrandom(long min, long max) {
  return (random() % (max - min)) + min;
}


/** Hides or shows the default mouse cursor over the screen.
Returns the previous state of the mouse cursor. */
int gyscreen_showcursor_(GyScreen * screen, int show) {
  int toggle = show ? SDL_ENABLE : SDL_DISABLE;
  return SDL_ShowCursor(toggle) == SDL_ENABLE;
}  

/** Returns true if the cursor is shown, false if not. */
int gyscreen_showcursor(GyScreen * screen) {  
  return SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE;
}


