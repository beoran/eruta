/** A mode is a specific mode in which the game can be.
* for example, menu mode, play mode, main screen, edit mode, etc. 
* In every mode, inputs and drawing may work in a different way.
*/
#ifndef MODE_H
#define MODE_H

#include "eruta.h"



/** Planned modes. */
enum ErutaModes_ {
  ERUTA_MODE_INTRO      = 1,
  ERUTA_MODE_MAINMENU   = 2,
  ERUTA_MODE_EXPLORE    = 3,
  ERUTA_MODE_BATTLE     = 4,
  ERUTA_MODE_DIALOG     = 5,
  ERUTA_MODE_MENU       = 6,
  ERUTA_MODE_LOAD       = 7,
  ERUTA_MODE_SAVE       = 8,
  ERUTA_MODE_NEW        = 9,
  ERUTA_MODE_TEST       = 10,
  ERUTA_MODE_MAX        = 16,
};

/**
* A Mode is a mode of the program in which the display and controls may 
* be different. Examples of this may be a menu mode, play mode,
* edit mode, intro mode, etc.
* 
*/
struct Mode_;
typedef struct Mode_ Mode;


/**
* ModeActs contains the actions , that is the function poMode *ers of a mode.
* These consist of start, enter, leave, done, event and paMode *
*/
struct ModeActs_;
typedef struct ModeActs_ ModeActs;


typedef Mode * (ModeActEvent)(Mode * self, ALLEGRO_EVENT   * event);
typedef Mode * (ModeActPaint)(Mode * self, ALLEGRO_DISPLAY * display);
typedef Mode * (ModeActMove)(Mode * self, Mode * other);
typedef Mode * (ModeActSelf)(Mode * self);

struct ModeActs_ {
  ModeActSelf   * start;
  ModeActMove   * enter;
  ModeActMove   * leave;
  ModeActPaint  * paint;
  ModeActEvent  * event;
  ModeActSelf   * stop;
};

#include "state.h"
#include "react.h"


/* This file was generated with:
'cfunctions -c -aoff -n -w mode_proto src/mode.c' */
#ifndef CFH_MODE_PROTO
#define CFH_MODE_PROTO

/* From 'src/mode.c': */

Mode * mode_start (Mode * self );

Mode * mode_enter (Mode * self , Mode * other );

Mode * mode_leave (Mode * self , Mode * other );

Mode * mode_stop (Mode * self );

Mode * mode_paint (Mode * self , ALLEGRO_DISPLAY * display );

Mode * mode_event (Mode * self , ALLEGRO_EVENT * event );

Mode * mode_init (Mode * self , State * state , ModeActs * acts , int id );

Mode * elsemode_start (Mode * self );

Mode * elsemode_enter (Mode * self , Mode * other );

Mode * elsemode_leave (Mode * self , Mode * other );

Mode * elsemode_stop (Mode * self );

Mode * elsemode_paint (Mode * self , ALLEGRO_DISPLAY * display );

Mode * elsemode_event (Mode * self , ALLEGRO_EVENT * event );

extern ModeActs elsemode_acts_;

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
ModeActs * elsemode_getacts PROTO ((void));

React * mode_react (Mode * self );

#endif /* CFH_MODE_PROTO */



#endif
