
#include "eruta.h"
#include "mode.h"



/*
* A Mode is a mode of the program in which the display and controls may 
* be different. Examples of this may be a menu mode, play mode,
* edit mode, intro mode, etc.
* 
*/

struct Mode_ {
  ModeActs *  acts;
  int         id;
  int         busy;
  Mode     *  prev; // mode left to enter this mode, if any
  Mode     *  next; // mode to leave to, if any
};

#define ACT_OK(SELF, ACT)  ((self) && (self->acts) && (self->acts->ACT))
          
Mode * mode_start(Mode * self) {
  if(!ACT_OK(self, start)) return NULL;
  return self->acts->start(self);
}

Mode * mode_enter(Mode * self, Mode * other) {
  if(!ACT_OK(self, enter)) return NULL;
  return self->acts->enter(self, other);
}

Mode * mode_leave(Mode * self, Mode * other) {
  if(!ACT_OK(self, leave)) return NULL;
  return self->acts->leave(self, other);
}

Mode * mode_stop(Mode * self) {
  if(!ACT_OK(self, stop)) return NULL;
  return self->acts->stop(self);
}

Mode * mode_paint(Mode * self, ALLEGRO_DISPLAY * display) {
  if(!ACT_OK(self, paint)) return NULL;
  return self->acts->paint(self, display);
}

Mode * mode_event(Mode * self, ALLEGRO_EVENT * event) {
  if(!ACT_OK(self, event)) return NULL;
  return self->acts->event(self, event);
}


Mode * mode_init(Mode * self, ModeActs * acts, int id) {
  self->acts = acts;
  self->next = NULL;
  self->prev = NULL;
  self->id   = id;
  self->busy = true;  
  return mode_start(self);
}

/* default "else" mode. */
Mode * elsemode_start(Mode * self) {
  return self;
}

Mode * elsemode_enter(Mode * self, Mode * other) {
  self->prev = other;
  return other;
}

Mode * elsemode_leave(Mode * self, Mode * other) {
  self->next = other;
  return other;
}

Mode * elsemode_stop(Mode * self) {  
  return self;
}

Mode * elsemode_paint(Mode * self, ALLEGRO_DISPLAY * display) {
  return NULL;
}

Mode * elsemode_event(Mode * self, ALLEGRO_EVENT * event) {
  return NULL;
}

ModeActs elsemode_acts_ = {
  elsemode_start,
  elsemode_enter,
  elsemode_leave,
  elsemode_paint,
  elsemode_event,
  elsemode_stop
};

ModeActs * elsemode_getacts() {
  return &elsemode_acts_;
};






