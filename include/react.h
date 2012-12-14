#ifndef react_H_INCLUDED
#define react_H_INCLUDED

#include "eruta.h"


typedef struct React_ React;

/* This file was generated with:
'cfunctions -c -aoff -n -w react_proto src/react.c' */
#ifndef CFH_REACT_PROTO
#define CFH_REACT_PROTO

/* From 'src/react.c': */

React * react_initempty (React * self , void * data );

React * react_react (React * self , ALLEGRO_EVENT * event );

React * react_poll (React * self , void * state );

#endif /* CFH_REACT_PROTO */

typedef React * (Reactor)(React * self, ALLEGRO_EVENT event);

/* Reactor function for joystick events. */
typedef React * (ReactJoystick)
(React * self, ALLEGRO_JOYSTICK_EVENT * event);

/* Reactor function for keyboard events. */
typedef React * (ReactKeyboard)
(React * self, ALLEGRO_KEYBOARD_EVENT * event);


/* Reactor function for individual key presses. */
typedef React * (ReactKey)
(React * self, int keycode);

/* Reactor function for mouse events. */
typedef React * (ReactMouse)
(React * self, ALLEGRO_MOUSE_EVENT * event);

/* Reactor function for timer events. */
typedef React * (ReactTimer)
(React * self, ALLEGRO_TIMER_EVENT * event);

/* Reactor function for display events. */
typedef React * (ReactDisplay)
(React * self, ALLEGRO_DISPLAY_EVENT * event);


/** React enables reaction handling to user input in ALLEGRO_EVENT events */
struct React_ {
  void              * data;
  ReactJoystick     * joystick_axis;
  ReactJoystick     * joystick_button_up;
  ReactJoystick     * joystick_button_down;
  ReactJoystick     * joystick_configuration;
  ReactKeyboard     * keyboard_key_down;
  ReactKeyboard     * keyboard_key_up;
  ReactKeyboard     * keyboard_key_char;
  ReactMouse        * mouse_axes;
  ReactMouse        * mouse_button_up;
  ReactMouse        * mouse_button_down;
  ReactMouse        * mouse_enter_display;
  ReactMouse        * mouse_leave_display;
  ReactMouse        * mouse_warped;
  ReactTimer        * timer;
  ReactDisplay      * display_expose;
  ReactDisplay      * display_resize;
  ReactDisplay      * display_close;
  ReactDisplay      * display_lost;
  ReactDisplay      * display_found;
  ReactDisplay      * display_switch_in;
  ReactDisplay      * display_switch_out;
  ReactDisplay      * display_orientation;

  ReactKey          * key_down[ALLEGRO_KEY_MAX];
  ReactKey          * key_up[ALLEGRO_KEY_MAX];
};





#endif




