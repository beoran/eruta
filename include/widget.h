#ifndef widget_H_INCLUDED
#define widget_H_INCLUDED

#include "eruta.h"
#include "image.h"
#include "rebox.h"




/** Style describes the style of a widget. */
struct Style_ {
  Color   forecolor;
  Color   backcolor;
  Font  * font;
  Image * background;
};

typedef struct Style_ Style;




/** Predefine Widget typedef. */
typedef struct Widget_ Widget;

/** Predefine Console typedef. */
typedef struct Console_ Console;


/** Predefine widget method table. */
typedef struct WidgetMetab_ WidgetMetab;

/** Predefine widget state flags . */
typedef enum WidgetFlags_ WidgetFlags;


/** Widget flags. A widget can be : visible or not
* listening (accepting input) input or not (if it  invisble and not listening 
* input it's inactive, otherwise active) 
* it can have focus or not (no focus listens to input 
* but may ignore it, focus accepts input).
* Selected is for checkboxes that are selected or buttons that are down, etc...
* 
*/
enum WidgetFlags {
  WIDGET_FLAG_VISIBLE    = 1,
  WIDGET_FLAG_LISTENING  = 2,
  WIDGET_FLAG_ACTIVE     = 3,
  WIDGET_FLAG_FOCUSED    = 4,
  WIDGET_FLAG_SELECTED   = 8,
};

/** Flag testing macros. */

/** Checks if flag is set.
Flag will be evaluated 2 times so must be a constant or a variable that is 
only read. 
*/
#define WIDGET_FLAG_P(WIDGET, FLAG) (((WIDGET)->flag&(FLAG))==(FLAG))

#define WIDGET_VISIBLE_P(WIDGET, FLAG) (((WIDGET)->flag&(FLAG))==(FLAG))

#define WIDGET_HANDLE_OK      0
#define WIDGET_HANDLE_IGNORE  1
#define WIDGET_HANDLE_ERROR  -1

/** Widget method table */
struct WidgetMetab_ {
  void *    (*free)   (void   * ptr);
  void *    (*done)   (void   * ptr);
  void      (*draw)   (Widget * self);
  int       (*handle) (Widget * self, ALLEGRO_EVENT * event);
  int       (*update) (Widget * self, double dt);
};

/** Widget interface */
struct WidgetIfa_ {
  Widget      * self;
  WidgetMetab * metab;
};


/** Widgets are individual parts of the UI.  
As a simplification, Widgets are considered to occupy "panes" ordered 
in the user interface from back to front. They do not contain other widgets 
and do not have any generic relations between them.

A note on pointer ownership: the pointers to font and image in style
are NOT cleaned up, since style is intended to be mostly a shallow copy in which
font and background image are repeated many times.
*/
struct Widget_ {
  /* Methods           */
  WidgetMetab   * metab;
  /* Bounds, this is a rectangular box. */
  Rebox           bounds;
  /* Styling elements. */
  Style           style;
  /* Widget elements: */
  /* Unique ID. */
  int id;
  /* Flags (active, disabled, etc) */
  int flags;
  /* Priority of widget */
  int z;
};

// Console input handler, called when a line of text (a command)
// is typed.
typedef int (ConsoleCommand)
            (Console * console, const char * command, void * extra);


/* This file was generated with:
'cfunctions -c -aoff -n -w widget_proto src/widget.c' */
#ifndef CFH_WIDGET_PROTO
#define CFH_WIDGET_PROTO

/* From 'src/widget.c': */

Style style_make (Color fore , Color back , Font * font , Image * background );

Style * style_initstyle (Style * self , Style style );

Style * style_init (Style * self , Color fore , Color back , Font * font , Image * background );

Color style_forecolor (Style * self );

Color style_backcolor (Style * self );

Image * style_background (Style * self );

Font * style_font (Style * self );

Rebox widget_bounds (Widget * self );

int widget_w (Widget * self );

int widget_h (Widget * self );

int widget_x (Widget * self );

int widget_y (Widget * self );

int widget_z (Widget * self );

Color widget_forecolor (Widget * self );

Color widget_backcolor (Widget * self );

Font * widget_font (Widget * self );

Image * widget_background (Widget * self );

int widget_flags (Widget * self );

int widget_id (Widget * self , int id );

int widget_flags_ (Widget * self , int flags );

int widget_id_ (Widget * self , int id );

int widget_flag (Widget * self , int flag );

int widget_unflag (Widget * self , int flag );

int widget_doflag (Widget * self , int flag , int set );

int widget_flag_p (Widget * self , int flag );

int widget_visible (Widget * self );

int widget_listening (Widget * self );

int widget_active (Widget * self );

int widget_focused (Widget * self );

int widget_selected (Widget * self );

int widget_visible_ (Widget * self , int set );

int widget_listening_ (Widget * self , int set );

int widget_active_ (Widget * self , int set );

int widget_focused_ (Widget * self , int set );

int widget_selected_ (Widget * self , int set );

Widget * widget_metab_ (Widget * self , WidgetMetab * metab );

Widget * widget_initall (Widget * self , int id , WidgetMetab * metab , Rebox bounds , Style style );

Widget * widget_initbounds (Widget * self , int id , WidgetMetab * metab , Rebox bounds );

Widget * widget_initparent (Widget * self , int id , Widget * parent );

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
Widget * widget_allocate PROTO ((void));

Widget * widget_done (Widget * widget );

Widget * widget_free (Widget * self );

void widget_draw (Widget * self );

void widget_handle (Widget * self , ALLEGRO_EVENT * event );

void widget_update (Widget * self , ALLEGRO_EVENT * event );

void widget_drawroundframe (Widget * self );

void console_command_ (Console * self , ConsoleCommand * command , void * data );

int console_docommand (Console * self , const char * text );

int console_addstr (Console * self , char * str );

int console_puts (Console * self , const char * str );

void console_draw (Widget * widget );

void console_active_ (Console * self , int active );

int console_active (Console * self );

int console_scroll (Console * self , int direction );

void * console_done (void * widget );

void * console_free (void * widget );

Console * console_alloc PROTO ((void));

Console * console_initall (Console * self , int id , Rebox bounds , Style style );

Console * console_new (int id , Rebox bounds , Style style );

#endif /* CFH_WIDGET_PROTO */

typedef Widget * (WidgetDraw)(Widget * self);





#endif




