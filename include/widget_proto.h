/* This file was generated with:
'cfunctions -c -aoff -n -w widget_proto src/widget.c' */
#ifndef CFH_WIDGET_PROTO
#define CFH_WIDGET_PROTO

/* From 'src/widget.c': */

#line 61 "src/widget.c"
Bounds bounds_make (int x , int y , int w , int h );

#line 69 "src/widget.c"
Bounds * bounds_initbounds (Bounds * self , Bounds bounds );

#line 76 "src/widget.c"
Bounds * bounds_init (Bounds * self , int x , int y , int w , int h );

#line 81 "src/widget.c"
int bounds_x (Bounds * self );

#line 86 "src/widget.c"
int bounds_y (Bounds * self );

#line 91 "src/widget.c"
int bounds_w (Bounds * self );

#line 96 "src/widget.c"
int bounds_h (Bounds * self );

#line 101 "src/widget.c"
int bounds_z (Bounds * self );

#line 107 "src/widget.c"
Style style_make (Color fore , Color back , Font * font , Image * background );

#line 113 "src/widget.c"
Style * style_initstyle (Style * self , Style style );

#line 121 "src/widget.c"
Style * style_init (Style * self , Color fore , Color back , Font * font , Image * background );

#line 126 "src/widget.c"
Color style_forecolor (Style * self );

#line 130 "src/widget.c"
Color style_backcolor (Style * self );

#line 134 "src/widget.c"
Image * style_background (Style * self );

#line 138 "src/widget.c"
Font * style_font (Style * self );

#line 149 "src/widget.c"
Bounds widget_bounds (Widget * self );

#line 154 "src/widget.c"
int widget_w (Widget * self );

#line 158 "src/widget.c"
int widget_h (Widget * self );

#line 162 "src/widget.c"
int widget_x (Widget * self );

#line 166 "src/widget.c"
int widget_y (Widget * self );

#line 170 "src/widget.c"
int widget_z (Widget * self );

#line 175 "src/widget.c"
Color widget_forecolor (Widget * self );

#line 179 "src/widget.c"
Color widget_backcolor (Widget * self );

#line 183 "src/widget.c"
Font * widget_font (Widget * self );

#line 188 "src/widget.c"
Image * widget_background (Widget * self );

#line 193 "src/widget.c"
int widget_flags (Widget * self );

#line 198 "src/widget.c"
int widget_id (Widget * self , int id );

#line 203 "src/widget.c"
int widget_flags_ (Widget * self , int flags );

#line 208 "src/widget.c"
int widget_id_ (Widget * self , int id );

#line 214 "src/widget.c"
int widget_flag (Widget * self , int flag );

#line 219 "src/widget.c"
int widget_unflag (Widget * self , int flag );

#line 226 "src/widget.c"
int widget_doflag (Widget * self , int flag , int set );

#line 231 "src/widget.c"
int widget_flag_p (Widget * self , int flag );

#line 236 "src/widget.c"
int widget_visible (Widget * self );

#line 241 "src/widget.c"
int widget_listening (Widget * self );

#line 247 "src/widget.c"
int widget_active (Widget * self );

#line 252 "src/widget.c"
int widget_focused (Widget * self );

#line 257 "src/widget.c"
int widget_selected (Widget * self );

#line 262 "src/widget.c"
int widget_visible_ (Widget * self , int set );

#line 268 "src/widget.c"
int widget_listening_ (Widget * self , int set );

#line 273 "src/widget.c"
int widget_active_ (Widget * self , int set );

#line 278 "src/widget.c"
int widget_focused_ (Widget * self , int set );

#line 283 "src/widget.c"
int widget_selected_ (Widget * self , int set );

#line 288 "src/widget.c"
Widget * widget_metab_ (Widget * self , WidgetMetab * metab );

#line 297 "src/widget.c"
Widget * widget_initall (Widget * self , int id , WidgetMetab * metab , Bounds bounds , Style style );

#line 311 "src/widget.c"
Widget * widget_initbounds (Widget * self , int id , WidgetMetab * metab , Bounds bounds );

#line 319 "src/widget.c"
Widget * widget_initparent (Widget * self , int id , Widget * parent );

#line 324 "src/widget.c"
#line 1 "/usr/local/share/cfunctions/c-extensions.h"
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

#line 329 "src/widget.c"
Widget * widget_done (Widget * widget );

#line 337 "src/widget.c"
Widget * widget_free (Widget * self );

#line 347 "src/widget.c"
void widget_draw (Widget * self );

#line 356 "src/widget.c"
void widget_drawroundframe (Widget * self );

#line 422 "src/widget.c"
void console_command_ (Console * self , ConsoleCommand * command , void * data );

#line 429 "src/widget.c"
int console_docommand (Console * self , const char * text );

#line 438 "src/widget.c"
int console_addstr (Console * self , char * str );

#line 465 "src/widget.c"
int console_puts (Console * self , const char * str );

#line 483 "src/widget.c"
void console_draw (Widget * widget );

#line 521 "src/widget.c"
void console_active_ (Console * self , int active );

#line 527 "src/widget.c"
int console_active (Console * self );

#line 533 "src/widget.c"
int console_scroll (Console * self , int direction );

#line 544 "src/widget.c"
int console_handle_keychar (Console * self , ALLEGRO_EVENT * event );

#line 581 "src/widget.c"
int console_handle_mouseaxes (Console * self , ALLEGRO_EVENT * event );

#line 594 "src/widget.c"
int console_handle (Widget * widget , ALLEGRO_EVENT * event );

#line 625 "src/widget.c"
void * console_done (void * widget );

#line 641 "src/widget.c"
void * console_free (void * widget );

#line 649 "src/widget.c"
Console * console_alloc PROTO ((void));

#line 656 "src/widget.c"
Console * console_initall (Console * self , int id , Bounds bounds , Style style );

#line 683 "src/widget.c"
Console * console_new (int id , Bounds bounds , Style style );

#endif /* CFH_WIDGET_PROTO */
