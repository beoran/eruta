#ifndef STATE_PROTO_H
#define STATE_PROTO_H
/*
This file was autogenerated from src/state.c
by bin/genproto on 2012-01-21 21:07:19 +0100
Please do not hand edit.
*/

/** The data struct contains all global state and other data of the application.
*/
struct State_;
typedef struct State_ State;

/** Gets the global game data, or NULL if not set. */
State * state_get();

/** Sets the given game data as the global game data for this apllication.
* returns the *previous* game data pointer or NULL if it was not set yet.
*/
State * state_set(State * game);

/** Return's the game's active tile map. */
Tilemap * state_nowmap(State * game);

/** Return's the game's loading tile map. */
Tilemap * state_loadmap(State * game);

/** Allocates a game struct */
State * state_alloc();

/** Frees a game struct */
void state_free(State * game);

/** Sets error message for game and returns NULL. */
State * state_errmsg_(State * game, char * mesg);

/** Gets error message for game. */
char * state_errmsg(State * game);

/** Registers an event source for this game */
State * state_eventsource(State * game, ALLEGRO_EVENT_SOURCE * src);

/** Gets a color from the game' color list. NOT bound checked! */
ALLEGRO_COLOR state_color(State * game, int color);

/** Sets a color to the game' color list. NOT bound checked! */
ALLEGRO_COLOR state_color_f(State * game, int color,
                         float r, float g, float b, float a);

/** Initializes the game. It opens the screen, keyboards,
lua interpreter, etc. Get any error with state_errmsg if
this returns NULL. */
State * state_init(State * self, BOOL fullscreen);

/** Sets the game's busy status to false */
BOOL state_done(State * game);

/** Closes the game when it's done. */
State * state_kill(State * self);

/** Returns true if the game is busy false if not. */
BOOL state_busy(State * self);

/** Polls the game's event queue, and gets the next event and stores it in
* event. if it is available. Returns true if there is an event of false if
* not.
*/
int state_poll(State * game,  ALLEGRO_EVENT * event);

/** Return the game's default font. */
ALLEGRO_FONT * state_font(State * game);

/** Call this every frame to update the FPS and frames value */
void state_frames_update(State * game);

/** Returns the amount of frames rendered during this second. */
int state_frames(State * game);

/** Returns the FPS value. */
double state_fps(State * game);

#endif // STATE_PROTO_H

