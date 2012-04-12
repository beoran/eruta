
#include "mode.h"
#include "state.h"
#include "react.h"

/** Initializes the react structure so it does nothing at all in all cases. */
React * react_initempty(React * self, void * data) {
  if(!self) return NULL;
  self->data                  = data;
  self->joystick_axis         = NULL;
  self->joystick_button_up    = NULL;
  self->joystick_button_down  = NULL;
  self->joystick_configuration= NULL;
  self->keyboard_key_down     = NULL;
  self->keyboard_key_up       = NULL;
  self->keyboard_key_char     = NULL;
  self->mouse_axes            = NULL;
  self->mouse_button_up       = NULL;
  self->mouse_button_down     = NULL;
  self->mouse_enter_display   = NULL;
  self->mouse_leave_display   = NULL;
  self->mouse_warped          = NULL;
  self->timer                 = NULL;
  self->display_expose        = NULL;
  self->display_resize        = NULL;
  self->display_close         = NULL;
  self->display_lost          = NULL;
  self->display_found         = NULL;
  self->display_switch_in     = NULL;
  self->display_switch_out    = NULL;
  self->display_orientation   = NULL;  
  return self;
}


#define DO_REACT(SELF, EVENT, MEMBER, HANDLER)              \
  do {                                                      \
    if(!SELF->HANDLER) return NULL;                         \
    return self->HANDLER(SELF, &((EVENT)->MEMBER));         \
  } while(0)  

/** React to an allegro event. */
React * react_react(React * self, ALLEGRO_EVENT * event) {
  switch(event->type) {
    case ALLEGRO_EVENT_JOYSTICK_AXIS:
      DO_REACT(self, event, joystick, joystick_axis);
    case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
      DO_REACT(self, event, joystick, joystick_button_down);
    case ALLEGRO_EVENT_JOYSTICK_BUTTON_UP:
      DO_REACT(self, event, joystick, joystick_button_up);
    case ALLEGRO_EVENT_JOYSTICK_CONFIGURATION:
      DO_REACT(self, event, joystick, joystick_configuration);
    case ALLEGRO_EVENT_KEY_DOWN:
      DO_REACT(self, event, keyboard, keyboard_key_down);
    case ALLEGRO_EVENT_KEY_CHAR:
      DO_REACT(self, event, keyboard, keyboard_key_char);
    case ALLEGRO_EVENT_KEY_UP:
      DO_REACT(self, event, keyboard, keyboard_key_up);   
    case ALLEGRO_EVENT_MOUSE_AXES:
      DO_REACT(self, event, mouse, mouse_axes);
    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
      DO_REACT(self, event, mouse, mouse_button_down);
    case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
      DO_REACT(self, event, mouse, mouse_button_up);
    case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
      DO_REACT(self, event, mouse, mouse_enter_display);
    case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
      DO_REACT(self, event, mouse, mouse_leave_display);
    case ALLEGRO_EVENT_MOUSE_WARPED:
      DO_REACT(self, event, mouse, mouse_warped);
    case ALLEGRO_EVENT_TIMER:
      DO_REACT(self, event, timer, timer);      
    case ALLEGRO_EVENT_DISPLAY_EXPOSE:
      DO_REACT(self, event, display, display_expose);
    case ALLEGRO_EVENT_DISPLAY_RESIZE:
      DO_REACT(self, event, display, display_resize);
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
      DO_REACT(self, event, display, display_close);
    case ALLEGRO_EVENT_DISPLAY_LOST:
      DO_REACT(self, event, display, display_lost);
    case ALLEGRO_EVENT_DISPLAY_FOUND:
      DO_REACT(self, event, display, display_found);
    case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
      DO_REACT(self, event, display, display_switch_in);
    case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
      DO_REACT(self, event, display, display_switch_out);
    case ALLEGRO_EVENT_DISPLAY_ORIENTATION:
      DO_REACT(self, event, display, display_orientation);
    default:
      return NULL;
  }
}


/** Polls for allegro events and reacts to them. */
React * react_poll(React * self, State * state) {
  ALLEGRO_EVENT event;
  if(!self) return NULL;
  while(state_poll(state, &event)) {
    // React to all incoming events.
    react_react(self, &event);
  }
  return self;
}  


