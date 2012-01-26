
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


Mode * mode_init(Mode * self, ModeActs * acts) {
  self->acts = acts;
  mode_start(self);
  return self;
}


