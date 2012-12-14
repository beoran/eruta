/* This file was generated with:
'cfunctions -c -aoff -n -w area_proto src/area.c' */
#ifndef CFH_AREA_PROTO
#define CFH_AREA_PROTO

/* From 'src/area.c': */

#line 49 "src/area.c"
int thing_id (Thing * thing );

#line 57 "src/area.c"
int thing_id_ (Thing * thing , int newid );

#line 73 "src/area.c"
Thing * thing_z_ (Thing * self , int z );

#line 83 "src/area.c"
int thing_setflag (Thing * self , int flag );

#line 88 "src/area.c"
int thing_unsetflag (Thing * self , int flag );

#line 95 "src/area.c"
int thing_flag_ (Thing * self , int flag , int set );

#line 100 "src/area.c"
int thing_flag (Thing * self , int flag );

#line 105 "src/area.c"
Thing * thing_done (Thing * self );

#line 122 "src/area.c"
Thing * thing_free (Thing * self );

#line 130 "src/area.c"
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
Thing * thing_alloc PROTO ((void));

#line 139 "src/area.c"
cpShape * shape_rectnew (cpBody * body , cpFloat x , cpFloat y , cpFloat w , cpFloat h , cpFloat xoff , cpFloat yoff );

#line 148 "src/area.c"
int thing_static_p (Thing * self );

#line 162 "src/area.c"
Thing * thing_initgeneric (Thing * self , Area * area , int kind , int z , cpBody * body , cpShape * shape );

#line 196 "src/area.c"
Thing * thing_initstatic (Thing * self , Area * area , int kind , int x , int y , int z , int w , int h );

#line 218 "src/area.c"
Thing * thing_initdynamic (Thing * self , Area * area , int kind , int x , int y , int z , int w , int h );

#line 247 "src/area.c"
Thing * thing_newstatic (Area * area , int kind , int x , int y , int z , int w , int h );

#line 258 "src/area.c"
Thing * thing_newdynamic (Area * area , int kind , int x , int y , int z , int w , int h );

#line 271 "src/area.c"
Point thing_p (Thing * self );

#line 276 "src/area.c"
int thing_x (Thing * self );

#line 281 "src/area.c"
int thing_y (Thing * self );

#line 286 "src/area.c"
int thing_w (Thing * self );

#line 291 "src/area.c"
int thing_h (Thing * self );

#line 298 "src/area.c"
int thing_cx (Thing * self );

#line 303 "src/area.c"
int thing_cy (Thing * self );

#line 308 "src/area.c"
int thing_z (Thing * self );

#line 313 "src/area.c"
Point thing_v (Thing * self );

#line 318 "src/area.c"
int thing_vx (Thing * self );

#line 323 "src/area.c"
int thing_vy (Thing * self );

#line 328 "src/area.c"
void thing_v_ (Thing * self , Point v );

#line 333 "src/area.c"
void thing_vxy_ (Thing * self , int vx , int vy );

#line 339 "src/area.c"
void thing_vx_ (Thing * self , int vx );

#line 346 "src/area.c"
void thing_vy_ (Thing * self , int vy );

#line 353 "src/area.c"
void thing_p_ (Thing * self , Point p );

#line 358 "src/area.c"
void thing_deltap (Thing * self , Point delta );

#line 365 "src/area.c"
void thing_pxy_ (Thing * self , int x , int y );

#line 371 "src/area.c"
void thing_x_ (Thing * self , int x );

#line 378 "src/area.c"
void thing_y_ (Thing * self , int y );

#line 385 "src/area.c"
void thing_applyforce (Thing * thing , const Point f );

#line 390 "src/area.c"
void thing_applyimpulse (Thing * thing , const Point f );

#line 395 "src/area.c"
void thing_resetforces (Thing * thing );

#line 403 "src/area.c"
void thing_draw (Thing * self , Camera * camera );

#line 476 "src/area.c"
int area_maxthings (Area * area );

#line 481 "src/area.c"
Thing * area_thing (Area * area , int index );

#line 489 "src/area.c"
Thing * area_thing_ (Area * area , int index , Thing * set );

#line 499 "src/area.c"
int area_thingid (Area * self );

#line 518 "src/area.c"
Thing * area_addthing (Area * area , Thing * thing );

#line 534 "src/area.c"
cpBody * area_staticbody (Area * area );

#line 542 "src/area.c"
int area_deletething (Area * area , int index );

#line 553 "src/area.c"
Area * area_cleanupthings (Area * self );

#line 564 "src/area.c"
Area * area_emptythings (Area * self );

#line 575 "src/area.c"
Area * area_done (Area * self );

#line 589 "src/area.c"
Area * area_free (Area * self );

#line 597 "src/area.c"
Area * area_alloc PROTO ((void));

#line 602 "src/area.c"
Area * area_init (Area * self );

#line 617 "src/area.c"
Area * area_new PROTO ((void));

#line 623 "src/area.c"
cpSpace * area_space (Area * self );

#line 631 "src/area.c"
Thing * area_newstatic (Area * self , int kind , int x , int y , int z , int w , int h );

#line 643 "src/area.c"
Thing * area_newdynamic (Area * self , int kind , int x , int y , int z , int w , int h );

#line 655 "src/area.c"
void area_draw (Area * self , Camera * camera );

#line 665 "src/area.c"
void area_update (Area * self , double dt );

#line 675 "src/area.c"

#ifdef COMMENT_
int thing_track (Tracker * tracker , void * data );

#endif

#endif /* CFH_AREA_PROTO */
