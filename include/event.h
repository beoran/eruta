#ifndef event_H_INCLUDED
#define event_H_INCLUDED

#include "eruta.h"

ALLEGRO_EVENT * event_alloc(void);
ALLEGRO_EVENT * event_free (ALLEGRO_EVENT * self );
unsigned int event_type (Event * self );
double event_timestamp (Event * self );
int event_ismouse (Event * self );
int event_isdisplay (Event * self );
int event_isjoystick (Event * self );
int event_iskeyboard (Event * self );
int event_istimer (Event * self );
int event_x (Event * self );
int event_y (Event * self );
int event_width (Event * self );
int event_height (Event * self );
int event_orientation (Event * self );
int event_stick (Event * self );
int event_axis (Event * self );
float event_pos (Event * self );
int event_button (Event * self );
int event_keycode (Event * self );
int event_unichar (Event * self );
int event_modifiers (Event * self );
int event_repeat (Event * self );
int event_z (Event * self );
int event_w (Event * self );
int event_dx (Event * self );
int event_dy (Event * self );
int event_dz (Event * self );
int event_dw (Event * self );
float event_pressure (Event * self );
int64_t event_count (Event * self );
double event_error (Event * self );



#endif




