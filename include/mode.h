/** A mode is a specific mode in which the game can be.
* for example, menu mode, play mode, main screen, edit mode, etc. 
* In every mode, inputs and drawing may work in a different way.
*/
#ifndef MODE_H
#define MODE_H

#include "gy.h"
#include "eruta.h"
#include "program.h"


/** Game for this mode. */
GyGame * mode_game(Mode * self);
 
/** Screen for this mode. */
GyScreen * mode_screen(Mode * self);

/** UI for this mode. */
void * mode_ui(Mode * self); 

/** Font for this mode. */
GyFont * mode_font(Mode * self);

/** Allocate a new, unusable mode. */
Mode * mode_alloc();
/** Initialize a new mode. */
Mode * mode_init(Mode * self, Program * program, ProgramMode id); 

/** Allocate a new mode for the given program with the given ProgramMode id. */
Mode * mode_new(Program * program, ProgramMode id);

/** Frees a mode. */
Mode * mode_free(Mode * self);

/** Event handler function. */
typedef Mode * (ModeEventFunction)(Mode * self, GyEvent event);

/** Draw function. */
typedef Mode * (ModeDrawFunction)(Mode * self, GyScreen * screen); 
 
/** Update function, when logic is updated. Also used on start. */
typedef Mode * (ModeUpdateFunction)(Mode * self); 


/** ModeClass wraps the flexible pointer members of the Mode. */
struct ModeClass_;
typedef struct ModeClass_ ModeClass;

struct ModeClass_ {
  ModeDrawFunction   * draw;
  ModeUpdateFunction * update;
  ModeUpdateFunction * start;
  ModeUpdateFunction * stop;
  ModeEventFunction  * events[GY_EVENT_LAST];
};


/** 
* A mode can register a sqeparate event handler for every different 
* GyEvent.kind, so keyboard inpout can be separated from mouse input, etc.   
*/
Mode * mode_eventmethod_(Mode * self, uint8_t type, ModeEventFunction * func);
/** Other handlers. */
Mode * mode_drawmethod_(Mode * self, ModeDrawFunction * func);
Mode * mode_updatemethod_(Mode * self, ModeUpdateFunction * func);
Mode * mode_startmethod_(Mode * self, ModeUpdateFunction * func);
Mode * mode_stopmethod_(Mode * self, ModeUpdateFunction * func);

/** Handle the given event. */
Mode * mode_event(Mode *self, GyEvent event);

/** Draw the mode. */
Mode * mode_draw(Mode * self, GyScreen * screen);

/** Update the mode. */
Mode * mode_update(Mode * self);

/** Switch to the mode from another one. */
Mode * mode_start(Mode *self);

/** Stop the mode (temporarily), when it is switched to another mode. */
Mode * mode_stop(Mode *self);

/** Current mode ID.*/
ProgramMode mode_id(Mode *self);

/** Requested mode ID. */
ProgramMode mode_nextmode(Mode * self);

/** Set requested mode ID. */
ProgramMode mode_nextmode_(Mode * self, ProgramMode mode);




#endif
