#include <stdlib.h>

#include "gari.h"
#define GARI_INTERN_ONLY
#include "gari_intern.h"

/* Constructs drawing information */
GariDraw * gari_drawdata_init(
                              GariDraw * data, 
                              GariDrawFunction * func, 
                              GariColor color,
                              GariAlpha alpha
                             ) {
  data->draw  = func;
  data->color = color;
  data->alpha = alpha;
  return data; 
} 


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
  uint32_t     frames;
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
  return game;
} 

GariGame * gari_game_done(GariGame * game) {
  if (!game) { return NULL; } 
  game->screen = NULL;
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


/** Updates the game screen, FPS, etc. */
GariGame * gari_game_update(GariGame * game) {
  SDL_Flip((SDL_Surface *)game->screen);
  game->frames++;
}

/** Sets the frames of the game back to 0.*/
GariGame * gari_game_resetframes(GariGame * game) {  
  game->frames = 0;
}

/** Gets the total amount of frames displayed since the start of the game, 
   or since gari_game_resetframes was called. */
uint32_t gari_game_frames(GariGame * game) {
  return game->frames;
}


/** Quickly fills the image or screen with the given color */
void gari_image_fill(GariImage * image,  GariColor color) {
  SDL_FillRect((SDL_Surface *) image, NULL, (Uint32) color);
}

/** Rerturns the drawable image of the screen. */
GariImage * gari_screen_image(GariScreen * screen) {
  return &(screen->image);
}







