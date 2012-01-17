#include "mem.h"
#include "game.h"




struct Game_ {
  BOOL                  busy;
  BOOL                  fullscreen;
  int32_t               modeno;
  ALLEGRO_FONT        * font;
  ALLEGRO_COLOR         colors[GAME_COLORS];
  ALLEGRO_DISPLAY     * display;
  ALLEGRO_BITMAP      * bkg;
  ALLEGRO_EVENT_QUEUE * queue;
  char                * errmsg; 
};


Game * game_alloc() {
  return STRUCT_ALLOC(Game);
}

void game_free(Game * game) {
  STRUCT_FREE(game);
}

/** Sets error message for game and returns NULL. */
Game * game_errmsg_(Game * game, char * mesg) {
  game->errmsg = mesg;
  return NULL;
}

/** Gets error message for game. */
char * game_errmsg(Game * game) {
  return game->errmsg;
}


/** Registers an event source for this game */
Game * game_eventsource(Game * game, ALLEGRO_EVENT_SOURCE * src)  {
  al_register_event_source(game->queue, src);
  return game;
}

/** Gets a color from the game' color list. NOT bound checked! */
ALLEGRO_COLOR game_color(Game * game, int color) {
  return game->colors[color];
}

/** Sets a color to the game' color list. NOT bound checked! */
ALLEGRO_COLOR game_color_f(Game * game, int color,
                         float r, float g, float b, float a) {
  return game->colors[color] = al_map_rgba_f(r, g, b, a);
}



/** Initializes the game. It opens the screen, keyboards,
lua interpreter, etc. Get any error with game_errmsg if
this returns NULL. */
Game * game_init(Game * self, BOOL fullscreen) {
  if(!self) return NULL;
  self->busy = TRUE;
  self->fullscreen = fullscreen;
  game_errmsg_(self, "OK!");
  // Initialize Allegro 5 and addons
  if (!al_init()) {
    return game_errmsg_(self, "Could not init Allegro.");
  }
  al_init_image_addon();
  al_init_font_addon();
  al_init_primitives_addon();

  // Install the keyboard handler
  if (!al_install_keyboard()) {
    return game_errmsg_(self, "Error installing keyboard.\n");
  }

  if (!al_install_mouse()) {
    return game_errmsg_(self, "Error installing mouse.\n");
  }
  
  // Use full screen mode if needed.
  if(self->fullscreen) { 
    al_set_new_display_flags(ALLEGRO_FULLSCREEN | ALLEGRO_GENERATE_EXPOSE_EVENTS);
  } 
  // Create a window to display things on: 640x480 pixels.
  self->display = al_create_display(SCREEN_W, SCREEN_H);
  if (!self->display) {
    return game_errmsg_(self, "Error creating display.\n");
  }
  
  al_resize_display(self->display, SCREEN_W, SCREEN_H);


  self->font = al_load_font("data/font/fixed_font.tga", 0, 0);
  if (!self->font) {
    return game_errmsg_(self, "Error loading data/font/fixed_font.tga");
  }

  game_color_f(self, ERUTA_WHITE, 1, 1, 1, 1);
  game_color_f(self, ERUTA_BLACK, 0, 0, 0, 1);
  // Start the event queue to handle keyboard input and our timer
  self->queue = al_create_event_queue();  
  game_eventsource(self, al_get_keyboard_event_source());
  game_eventsource(self, al_get_display_event_source(self->display));
  game_eventsource(self, al_get_mouse_event_source());
  al_set_window_title(self->display, "Eruta!");  
  return self;
}

/** Sets the game's busy status to false */
BOOL game_done(Game * game) {
  game->busy = FALSE;
  return game->busy;
}

/** Closes the game when it's done. */
Game * game_kill(Game * self) {
  STRUCT_FREE(self);
  return self;
}

/** Returns true if the game is busy false if not. */
BOOL game_busy(Game * self) {
  return self->busy;
}

/** Polls the game's event queue, and gets the next event and stores it in
* event. if it is available. Returns true if there is an event of false if
* not.
*/

int game_poll(Game * game,  ALLEGRO_EVENT * event) {
  return al_get_next_event(game->queue, event);
}  




