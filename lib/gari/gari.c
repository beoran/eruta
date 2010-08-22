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


struct GariGame_ {
  GariScreen * screen;
  int          error;
  char *       message;
  /* For FPS calculation. */  
  uint32_t     frames;  
  uint32_t     framestart;
  uint32_t     framestop;
  uint32_t     delta;
  SDL_Joystick **joysticks;
};


GariGame * gari_game_make() {
  GariGame * game = GARI_MALLOC(sizeof(GariGame));
  if(gari_game_init(game)) { return game; }
  gari_game_free(game);
  return NULL;  
}

/** describes a joystick. */
void gari_game_showjoystick(SDL_Joystick * joy, int index) {  
  if(!joy) { 
    fprintf(stderr, "Could not open joystick nr %d!\n", index);
    return;
  }    
  fprintf(stderr, "Joystick Name: %s\n", SDL_JoystickName(index));
  fprintf(stderr, "Axes: %d ", SDL_JoystickNumAxes(joy));
  fprintf(stderr, "Buttons: %d ", SDL_JoystickNumButtons(joy));
  fprintf(stderr, "Balls: %d \n", SDL_JoystickNumBalls(joy));
}

/*
if(SDL_NumJoysticks()>0){
  joy=SDL_JoystickOpen(0);
  
  if(joy)
  {
    printf("Opened Joystick 0\n");
    printf("Name: %s\n", SDL_JoystickName(0));
    printf("Number of Axes: %d\n", SDL_JoystickNumAxes(joy));
    printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));
    printf("Number of Balls: %d\n", SDL_JoystickNumBalls(joy));
*/

// opens all available joysticks.
GariGame * gari_game_openjoysticks(GariGame * gari) {
  int index;
  if(SDL_NumJoysticks()<1) return NULL;
  gari->joysticks = GARI_MALLOC(sizeof(SDL_Joystick *) * SDL_NumJoysticks());
  for(index = 0; index < SDL_NumJoysticks(); index ++) {
    gari->joysticks[index] = SDL_JoystickOpen(index);
    gari_game_showjoystick(gari->joysticks[index], index);
  }
  return gari;
}

// closes all available joysticks.
GariGame * gari_game_closejoysticks(GariGame * gari) {
  int index;
  if(SDL_NumJoysticks()<1) return NULL;
  if(!(gari->joysticks)) return NULL;
  for(index = 0; index < SDL_NumJoysticks(); index ++) {
    SDL_JoystickClose(gari->joysticks[index]);        
  }
  GARI_FREE(gari->joysticks);
  gari->joysticks 	= NULL;
  return gari;
}


/** Returns how many joysticks are available. */
int gari_game_joysticks(GariGame * game) {
  return SDL_NumJoysticks();
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
  
  
  
  TTF_Quit();
  SDL_Quit();
  return game;
}

void gari_game_free(GariGame * game) {  
  gari_free(gari_game_done(game));  
}


GariScreen * gari_game_screen(GariGame * game) {
  return game->screen;
}


GariScreen * gari_screen_make(GariGame * game, int wide, int high, int fullscreen) {
  int     depth = GARI_DEFAULT_DEPTH, new_depth = 0;
  SDL_Surface * screen = NULL;
  Uint32  flags = 0;
  if (game->screen) { return game->screen; }
  flags         = SDL_HWSURFACE;
  if (fullscreen)   { flags     |= SDL_FULLSCREEN; }
  new_depth  = SDL_VideoModeOK(wide, high, depth, flags);
  if (!new_depth ) { 
    flags      = SDL_ANYFORMAT;
    new_depth  = SDL_VideoModeOK(wide, high, GARI_DEFAULT_DEPTH, flags);
    if(!new_depth) {
      fprintf(stderr, "\nUnable to open screen: %dx%d@%d : %s\n", wide, high, new_depth, SDL_GetError());
      return NULL;
    }
  }  
  screen = SDL_SetVideoMode(wide, high, new_depth, flags);
  if(!screen) {
      fprintf(stderr, "\nUnable to open screen: %dx%d@%d : %s\n", wide, high, new_depth, SDL_GetError());
      return NULL;
    }

  game->screen = (GariScreen *) screen;
  return game->screen;
}

/** Advances the frame counter of the game. */
GariGame * gari_game_nextframe(GariGame * game) {
  game->frames++;
  return game;
}

/** Updates the game screen, FPS, etc. */
GariGame * gari_game_update(GariGame * game) {
  SDL_Flip((SDL_Surface *)game->screen);
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
  uint32_t delta;
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

/** Quickly fills the image or screen with the given color */
void gari_image_fill(GariImage * image,  GariDye color) {
  SDL_FillRect((SDL_Surface *) image, NULL, (Uint32) color);
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






