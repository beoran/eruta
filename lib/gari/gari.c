#include <stdlib.h>

#include "gari.h"
#define GARI_INTERN_ONLY
#include "gari_intern.h"



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
};


GariGame * gari_game_make() {
  GariGame * game = malloc(sizeof(GariGame));
  if(gari_game_init(game)) { return game; }
  gari_game_free(game);
  return NULL;  
}

GariGame * gari_game_init(GariGame * game) {
  if (!game) { return NULL; }
  gari_game_resetframes(game);
  game->error  = 0;
  game->screen = NULL;
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
  return game;
} 

GariGame * gari_game_done(GariGame * game) {
  if (!game) { return NULL; } 
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
void gari_image_fill(GariImage * image,  GariColor color) {
  SDL_FillRect((SDL_Surface *) image, NULL, (Uint32) color);
}

/** Rerturns the drawable image of the screen. */
GariImage * gari_screen_image(GariScreen * screen) {
  return &(screen->image);
}







