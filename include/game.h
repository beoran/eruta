#ifndef ERUTA_GAME_H
#define ERUTA_GAME_H

#include "eruta.h"

#define GAME_COLORS   16
#define ERUTA_BLACK   0
#define ERUTA_WHITE   1
/** Amount of samples that can be played tah the same time. */
#define ERUTA_SAMPLES 16

/** The game struct contains all global state of the eruta game. */
struct Game_;
typedef struct Game_ Game;

/** Initializes the game. It opens the screen, keyboards,
lua interpreter, etc. */
Game * game_init(Game * game, BOOL fullscreen);

/** Closes the game when it's done. */
Game * game_kill(Game * game);

/** Returns true if the game is busy false if not. */
BOOL game_busy();

/** Allocates the game's data. */
Game * game_alloc();

/** Frees the game's data. */
void game_free(Game * game);

/** Sets error message for game and returns NULL. */
Game * game_errmsg_(Game * game, char * mesg);

/** Gets error message for game. */
char * game_errmsg(Game * game);

/** Registers an event source for this game */
Game * game_eventsource(Game * game, ALLEGRO_EVENT_SOURCE * src);

/** Sets the game's busy status to false */
BOOL game_done(Game * game);

/** Polls the game's event queue, and gets the next event and stores it in
* event. if it is available. Returns true if there is an event of false if
* not.
*/
BOOL game_poll(Game * game,  ALLEGRO_EVENT * event);

/** Gets a color from the game' color list. NOT bound checked! */
ALLEGRO_COLOR game_color(Game * game, int color);

/** Sets a color to the game' color list. NOT bound checked! */
ALLEGRO_COLOR game_color_f(Game * game, int color,
                         float r, float g, float b, float a);



#endif
