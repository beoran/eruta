#ifndef STATE_PROTO_H
#define STATE_PROTO_H
/*
This file was autogenerated from src/state.c
by bin/genproto
Please do not hand edit.
*/

/** The data struct contains all global state and other data of the application.
*/
struct State_;
typedef struct State_ State;

/** Gets the global state data, or NULL if not set. */
State * state_get();

/** Sets the given state data as the global state data for this apllication.
* returns the *previous* state data pointer or NULL if it was not set yet.
*/
State * state_set(State * state);

/** Return's the state's active tile map. */
Tilemap * state_nowmap(State * state);

/** Return's the state's loading tile map. */
Tilemap * state_loadmap(State * state);

/** Allocates a state struct */
State * state_alloc();

/** Frees a state struct */
void state_free(State * state);

/** Sets error message for state and returns NULL. */
State * state_errmsg_(State * state, char * mesg);

/** Gets error message for state. */
char * state_errmsg(State * state);

/** Registers an event source for this state */
State * state_eventsource(State * state, ALLEGRO_EVENT_SOURCE * src);

/** Gets a color from the state' color list. NOT bound checked! */
ALLEGRO_COLOR state_color(State * state, int color);

/** Sets a color to the state' color list. NOT bound checked! */
ALLEGRO_COLOR state_color_f(State * state, int color,
                         float r, float g, float b, float a);

/** Initializes the state. It opens the screen, keyboards,
lua interpreter, etc. Get any error with state_errmsg if
this returns NULL. */
State * state_init(State * self, BOOL fullscreen);

/** Sets the state's busy status to false */
BOOL state_done(State * state);

/** Closes the state when it's done. */
State * state_kill(State * self);

/** Returns true if the state is busy false if not. */
BOOL state_busy(State * self);

/** Polls the state's event queue, and gets the next event and stores it in
* event. if it is available. Returns true if there is an event of false if
* not.
*/
int state_poll(State * state,  ALLEGRO_EVENT * event);

/** Return the state's default font. */
ALLEGRO_FONT * state_font(State * state);

/** Call this every frame to update the FPS and frames value */
void state_frames_update(State * state);

/** Returns the amount of frames rendered during this second. */
int state_frames(State * state);

/** Returns the FPS value. */
double state_fps(State * state);

/** Returns the camera of the state. */
Camera * state_camera(State * state);

#endif // STATE_PROTO_H

