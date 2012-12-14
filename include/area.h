#ifndef area_H_INCLUDED
#define area_H_INCLUDED

#include <chipmunk.h>



/**
An area is an in game region that forms a single unity in which play
takes place. Ok, its' what you'd call a "level". Every area
consists of a single tile map that determines the visual looks of the area,
a chipmunk cpSpace that simulates the dynamics, sprite information
that determines the visibility and position of the sprites, and a reference
to the scripting engine that contains the logical state of the game.

Division of the data locations: visual and physics engine: in C.
Logic/game/character data: in scripting engine.
*/
struct Area_;
typedef struct Area_ Area;

typedef struct Thing_ Thing; 



/* Thing types. Generally speaking, there are 
things that can move and those that cannot. 
Chipmunks treats them differently in the sense that
static things will all use a single (or one of 
a few) static bodies, which does not need to be 
freed when the thing isn't needed anymore.
*/
#define THING_UNUSED -1
#define THING_WALL   1
#define THING_WATER  2
#define THING_STAIR  3
#define THING_ACTOR  4
#define THING_MOBILE 5
#define THING_ZONE   6


/** Need the camera and it's prototypes. */
#include "camera.h"

/* This file was generated with:
'cfunctions -c -aoff -n -w area_proto src/area.c' */
#ifndef CFH_AREA_PROTO
#define CFH_AREA_PROTO

/* From 'src/area.c': */

int thing_id (Thing * thing );

int thing_id_ (Thing * thing , int newid );

Thing * thing_z_ (Thing * self , int z );

int thing_setflag (Thing * self , int flag );

int thing_unsetflag (Thing * self , int flag );

int thing_flag_ (Thing * self , int flag , int set );

int thing_flag (Thing * self , int flag );

Thing * thing_done (Thing * self );

Thing * thing_free (Thing * self );

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

cpShape * shape_rectnew (cpBody * body , cpFloat x , cpFloat y , cpFloat w , cpFloat h , cpFloat xoff , cpFloat yoff );

int thing_static_p (Thing * self );

Thing * thing_initgeneric (Thing * self , Area * area , int kind , int z , cpBody * body , cpShape * shape );

Thing * thing_initstatic (Thing * self , Area * area , int kind , int x , int y , int z , int w , int h );

Thing * thing_initdynamic (Thing * self , Area * area , int kind , int x , int y , int z , int w , int h );

Thing * thing_newstatic (Area * area , int kind , int x , int y , int z , int w , int h );

Thing * thing_newdynamic (Area * area , int kind , int x , int y , int z , int w , int h );

Point thing_p (Thing * self );

int thing_x (Thing * self );

int thing_y (Thing * self );

int thing_w (Thing * self );

int thing_h (Thing * self );

int thing_cx (Thing * self );

int thing_cy (Thing * self );

int thing_z (Thing * self );

Point thing_v (Thing * self );

int thing_vx (Thing * self );

int thing_vy (Thing * self );

void thing_v_ (Thing * self , Point v );

void thing_vxy_ (Thing * self , int vx , int vy );

void thing_vx_ (Thing * self , int vx );

void thing_vy_ (Thing * self , int vy );

void thing_p_ (Thing * self , Point p );

void thing_deltap (Thing * self , Point delta );

void thing_pxy_ (Thing * self , int x , int y );

void thing_x_ (Thing * self , int x );

void thing_y_ (Thing * self , int y );

void thing_applyforce (Thing * thing , const Point f );

void thing_applyimpulse (Thing * thing , const Point f );

void thing_resetforces (Thing * thing );

void thing_draw (Thing * self , Camera * camera );

int area_maxthings (Area * area );

Thing * area_thing (Area * area , int index );

Thing * area_thing_ (Area * area , int index , Thing * set );

int area_thingid (Area * self );

Thing * area_addthing (Area * area , Thing * thing );

cpBody * area_staticbody (Area * area );

int area_deletething (Area * area , int index );

Area * area_cleanupthings (Area * self );

Area * area_emptythings (Area * self );

Area * area_done (Area * self );

Area * area_free (Area * self );

Area * area_alloc PROTO ((void));

Area * area_init (Area * self );

Area * area_new PROTO ((void));

cpSpace * area_space (Area * self );

Thing * area_newstatic (Area * self , int kind , int x , int y , int z , int w , int h );

Thing * area_newdynamic (Area * self , int kind , int x , int y , int z , int w , int h );

void area_draw (Area * self , Camera * camera );

void area_update (Area * self , double dt );


#ifdef COMMENT_
int thing_track (Tracker * tracker , void * data );

#endif

#endif /* CFH_AREA_PROTO */


#endif




