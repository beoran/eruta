#include <stdio.h>
#include "si_macro.h"
#include "si_mem.h"
#include "mode.h"
    

struct Mode_ {
  Program    *  program;
  ProgramMode   id;                 ; // Mode ID
  ProgramMode   next;               ; // Next requested mode's ID
  ModeClass     klass               ; // Flexible event handlers.
};

ModeClass * modeclass_init(ModeClass * self) {
  int index;
  for (index = 0; index < GARI_EVENT_LAST; index ++) { 
    self->events[index] = NULL;
  }  
  self->start   = NULL;
  self->stop    = NULL;
  self->draw    = NULL;
  self->update  = NULL; 
  return self;
}


Mode * mode_init(Mode * self, Program * program, ProgramMode id) {
  if (!self) return NULL;
  self->program = program;
  self->id      = id;
  modeclass_init(&self->klass);
  self->next    = ProgramModeNone;
  return self;  
};

// Game for this mode .
GariGame * mode_game(Mode * self) { 
  return program_game(self->program);
}  

GariScreen * mode_screen(Mode * self) { 
  return program_screen(self->program);
}
  
void * mode_ui(Mode * self) { 
  return program_ui(self->program);
}

GariFont * mode_font(Mode * self) { 
  return program_font(self->program);
}

Mode * mode_alloc() {
  return SI_ALLOCATE(Mode);
}

Mode * mode_new(Program * program, ProgramMode id) {
  return mode_init(mode_alloc(), program, id);
}

Mode * mode_free(Mode * self) {
  return si_free(self);
}

/** 
* A mode can register a sqeparate event handler for every different 
* GariEvent.kind, so keyboard inpout can be separated from mouse input, etc.   
*/
Mode * mode_eventmethod_(Mode * self, uint8_t type, ModeEventFunction * func) {
  self->klass.events[type] = func;
  return self;
}

/** Other handlers. */
Mode * mode_drawmethod_(Mode * self, ModeDrawFunction * func) {
  self->klass.draw = func;
  return self;
}

Mode * mode_updatemethod_(Mode * self, ModeUpdateFunction * func) {
  self->klass.update = func;
  return self;
}

Mode * mode_startmethod_(Mode * self, ModeUpdateFunction * func) { 
  self->klass.start = func;
  return self;
}

Mode * mode_stopmethod_(Mode * self, ModeUpdateFunction * func) { 
  self->klass.stop  = func;
  return self;
}

/** Handle the given event. */
Mode * mode_event(Mode *self, GariEvent event) {
  ModeEventFunction * func;
  if((!self) || (event.kind <= GARI_EVENT_NONE)) return NULL;
  if((event.kind >= GARI_EVENT_LAST)) return NULL;
  func = self->klass.events[event.kind];
  // Ignore unhandled event 
  if(!func) return self;
  // Call event handler. 
  return func(self, event);
}

/** Draw the mode. */
Mode * mode_draw(Mode * self, GariScreen * screen) {
  ModeDrawFunction * func;
  func = self->klass.draw;
  // Can't draw now. 
  if(!func) return NULL;
  return func(self, screen); 
}

/** Update the mode. */
Mode * mode_update(Mode * self) {
  ModeUpdateFunction * func = self->klass.update; 
  if(!func) return NULL;
  return func(self); 
}

/** Switch to the mode from another one. */
Mode * mode_start(Mode *self) {
  ModeUpdateFunction * func = self->klass.start;
  // prevent eternal ping-pong loop
  mode_nextmode_(self, ProgramModeNone);  
  if(!func) return NULL;
  return func(self); 
}

/** Stop the mode (temporarily), when it is switched to another mode. */
Mode * mode_stop(Mode *self) {
  ModeUpdateFunction * func = self->klass.stop;
  // prevent eternal ping-pong loop
  mode_nextmode_(self, ProgramModeNone);  
  if(!func) return NULL;
  return func(self); 
}

/** Current mode ID.*/
ProgramMode mode_id(Mode *self) {
  return self->id;
}

/** Requested mode ID. Call only once, as it will auto-reset to ProgramModeNone. 
*/
ProgramMode mode_nextmode(Mode * self) {
  ProgramMode n = self->next;
  // prevent eternal ping-pong loop
  mode_nextmode_(self, ProgramModeNone);
  return n;
}

/** Set requested mode ID. */
ProgramMode mode_nextmode_(Mode * self, ProgramMode mode) {
  self->next = mode;
  return self->next;
}


