
#include "event.h"
#include "mem.h"





/** Helkper functions for Allegro's events. 
* This mostly exists for use with lua scripting. 
*/


/** Allocates a new allegro event */
ALLEGRO_EVENT * event_alloc() {
  return mem_alloc(sizeof(ALLEGRO_EVENT));
} 

/** Frees an allegro event that was allocated though event_alloc. 
Returns NULL;*/
ALLEGRO_EVENT * event_free(ALLEGRO_EVENT * self) {
  mem_free(self);
  return NULL;
}

/** Returns the type of an event, or 0 if not valid. */
unsigned int event_type(Event * self) {
  if(!self) return 0;
  return self->type;
}

/** Returns the datestamp of an event, or -1.0 if not valid. */
double event_timestamp(Event * self) {
  if(!self) return -1.0;
  return self->any.timestamp;
}

/** Returns nonzero if the event's type is mouse related,  zero if not.*/
int event_ismouse(Event * self) {
  if ((self->type >= ALLEGRO_EVENT_MOUSE_AXES)
  && (self->type <= ALLEGRO_EVENT_MOUSE_WARPED))
    return TRUE;
  return FALSE;
}

/** Returns nonzero if the event's type is display related,  zero if not.*/
int event_isdisplay(Event * self) {
  if ((self->type >= ALLEGRO_EVENT_DISPLAY_EXPOSE)
  && (self->type <= ALLEGRO_EVENT_DISPLAY_ORIENTATION))
    return TRUE;
  return FALSE;
}

/** Returns nonzero if the event's type is joystick related,  zero if not.*/
int event_isjoystick(Event * self) {
  if ((self->type >= ALLEGRO_EVENT_JOYSTICK_AXIS)
  && (self->type <= ALLEGRO_EVENT_JOYSTICK_CONFIGURATION))
    return TRUE;
  return FALSE;
}

/** Returns nonzero if the event's type is keyboard related, zero if not.*/
int event_iskeyboard(Event * self) {
  if ((self->type >= ALLEGRO_EVENT_KEY_DOWN)
  && (self->type <= ALLEGRO_EVENT_KEY_UP))
    return TRUE;
  return FALSE;
}

/** Returns nonzero if the event's type is keyboard related, zero if not.*/
int event_istimer(Event * self) {
  if (self->type == ALLEGRO_EVENT_TIMER) return TRUE;
  return FALSE;
}


/** Returns the x position of  an event (for mouse
and display event) return -1 if not valid. */
int event_x(Event * self) {
  if(!self) return -1;
  if(event_ismouse(self))   return self->mouse.x;
  if(event_isdisplay(self)) return self->display.x;
  return -1;
}


/** Returns the y position of  an event (for mouse
and display event) return -1 if not valid. */
int event_y(Event * self) {
  if(!self) return -1;
  if(event_ismouse(self))   return self->mouse.y;
  if(event_isdisplay(self)) return self->display.y;
  return -1;
}


/** Returns the width of an event (for display event) 
returns -1 if not valid. */
int event_width(Event * self) {
  if(!self) return -1;
  if(event_isdisplay(self)) return self->display.width;
  return -1;
}

/** Returns the height of an event (for display event) 
returns -1 if not valid. */
int event_height(Event * self) {
  if(!self) return -1;
  if(event_isdisplay(self)) return self->display.height;
  return -1;
}


/** Returns the orientation of an event (for display event) 
returns -1 if not valid. */
int event_orientation(Event * self) {
  if(!self) return -1;
  if(event_isdisplay(self)) return self->display.orientation;
  return -1;
}


/** Returns the stick number of a joystick event, -1 on error. */
int event_stick(Event * self) {
  if(!self) return -1;
  if(event_isjoystick(self)) return self->joystick.stick;
  return -1;
}

/** Returns the axis number of a joystick event, -1 on error.  */
int event_axis(Event * self) {
  if(!self) return -1;
  if(event_isjoystick(self)) return self->joystick.axis;
  return -1;
}

/** Returns the position of a joystick event, 0.0 on error  */
float event_pos(Event * self) {  
  if(!self) return -1;
  if(event_isjoystick(self)) return self->joystick.pos;
  return -1;
}

/** Returns the button number of a joystick event or mouse event, -1 on error  */
int event_button(Event * self) {
  if(!self) return -1;
  if(event_isjoystick(self)) return self->joystick.button;
  if(event_ismouse(self)) return (int) self->mouse.button;
  return -1;
}


/** returns the keycode for the keyboard event, -1 n error.*/
int event_keycode(Event * self) {
  if(!self) return -1;
  if(event_iskeyboard(self)) return self->keyboard.keycode;
  return -1;
}

/** returns the unicode character for the keyboard event, -1 n error.*/
int event_unichar(Event * self) {
  if(!self) return -1;
  if(event_iskeyboard(self)) return self->keyboard.unichar;
  return -1;
}

/** Returns the modifiers for the keyboard event, -1 n error.*/
int event_modifiers(Event * self) {
  if(!self) return -1;
  if(event_iskeyboard(self)) return (int)self->keyboard.modifiers;
  return -1;
}

/** Returns the autorepeat field of the keyboard event, -1 n error.*/
int event_repeat(Event * self) {
  if(!self) return -1;
  if(event_iskeyboard(self)) return (int)self->keyboard.repeat;
  return -1;
}


/** Returns the z for a mouse event, -1 on error */
int event_z(Event * self) {
  if(!self) return -1;
  if(event_ismouse(self)) return (int) self->mouse.z;
  return -1;
}

/** Returns the z for a mouse event, -1 on error */
int event_w(Event * self) {
  if(!self) return -1;
  if(event_ismouse(self)) return (int) self->mouse.w;
  return -1;
}


/** Returns the dx for a mouse event, -1 on error */
int event_dx(Event * self) {
  if(!self) return -1;
  if(event_ismouse(self)) return (int) self->mouse.dx;
  return -1;
}

/** Returns the dy for a mouse event, -1 on error */
int event_dy(Event * self) {
  if(!self) return -1;
  if(event_ismouse(self)) return (int) self->mouse.dy;
  return -1;
}

/** Returns the dz for a mouse event, -1 on error */
int event_dz(Event * self) {
  if(!self) return -1;
  if(event_ismouse(self)) return (int) self->mouse.dz;
  return -1;
}

/** Returns the dw for a mouse event, -1 on error */
int event_dw(Event * self) {
  if(!self) return -1;
  if(event_ismouse(self)) return (int) self->mouse.dw;
  return -1;
}

/** Returns the pressure for a mouse event, -1.0 on error */
float event_pressure(Event * self) {
  if(!self) return -1.0;
  if(event_ismouse(self)) return self->mouse.pressure;
  return -1.0;
}

/** Returns the count for a timer event, -1 on error */
int64_t event_count(Event * self) {
  if(!self) return -1;
  if(event_istimer(self)) return self->timer.count;
  return -1;
}

/** Returns the timer error for a timer event, 0.0 on error */
double event_error(Event * self) {
  if(!self) return  0.0;
  if(event_istimer(self)) return self->timer.error;
  return 0.0;
}
