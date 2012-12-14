#ifndef event_H_INCLUDED
#define event_H_INCLUDED

#include "eruta.h"
/* This file was generated with:
'cfunctions -c -aoff -n -w event_proto src/event.c' */
#ifndef CFH_EVENT_PROTO
#define CFH_EVENT_PROTO

/* From 'src/event.c': */

/* Macro definitions for C extensions for Cfunctions. */

/* 
   Copyright (C) 1998 Ben K. Bullock.

   This header file is free software; Ben K. Bullock gives unlimited
   permission to copy, modify and distribute it. 

   This header file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/


#ifndef C_EXTENSIONS_H
#define C_EXTENSIONS_H

/* Only modern GNU C's have `__attribute__'.  The keyword `inline'
   seems to have been around since the beginning, but it does not work
   with the `-ansi' option, which defines `__STRICT_ANSI__'.  I expect
   that `__attribute__' does not work with `-ansi' either.  Anyway
   both of these failure cases are being lumped together here for the
   sake of brevity. */

#if defined (__GNUC__) && ( __GNUC__ >= 2 ) && ( __GNUC_MINOR__ > 4 ) && \
   ! defined (__STRICT_ANSI__)

/* Macro definitions for Gnu C extensions to C. */

#define X_NO_RETURN __attribute__((noreturn))
#define X_PRINT_FORMAT(a,b) __attribute__((format(printf,a,b)))
#define X_CONST __attribute__((const))
#define X_INLINE

#else /* Not a modern GNU C */

#define X_NO_RETURN 
#define X_PRINT_FORMAT(a,b) 
#define X_CONST

#endif /* GNU C */

/* The following `#define' is not a mistake.  INLINE is defined to
   nothing for both GNU and non-GNU compilers.  When Cfunctions sees
   `INLINE' it copies the whole of the function into the header file,
   prefixed with `extern inline' and surrounded by an `#ifdef
   X_INLINE' wrapper.  In order to inline the function in GNU C, only
   `X_INLINE' needs to be defined. There is also a normal prototype
   for the case that X_INLINE is not defined.  The reason for copying
   the function with a prefix `extern inline' into the header file is
   explained in the GNU C manual and the Cfunctions manual. */

#define INLINE
#define NO_RETURN void
#define NO_SIDE_FX
#define PRINT_FORMAT(a,b)
#define LOCAL

/* Prototype macro for `traditional C' output. */

#ifndef PROTO
#if defined(__STDC__) && __STDC__ == 1
#define PROTO(a) a
#else
#define PROTO(a) ()
#endif /* __STDC__ */
#endif /* PROTO */

#endif /* ndef C_EXTENSIONS_H */
ALLEGRO_EVENT * event_alloc PROTO ((void));

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

#endif /* CFH_EVENT_PROTO */



#endif




