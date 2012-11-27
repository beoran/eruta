/* This file was generated with:
'cfunctions -c -aoff -n -w camera_proto src/camera.c' */
#ifndef CFH_CAMERA_PROTO
#define CFH_CAMERA_PROTO

/* From 'src/camera.c': */

#line 42 "src/camera.c"
int panner_setflag (Panner * self , int flag );

#line 47 "src/camera.c"
int panner_unsetflag (Panner * self , int flag );

#line 54 "src/camera.c"
int panner_flag_ (Panner * self , int flag , int set );

#line 59 "src/camera.c"
int panner_flag (Panner * self , int flag );

#line 67 "src/camera.c"
Panner * panner_init (Panner * self , Point goal , float speed , int immediate );

#line 78 "src/camera.c"
Panner * panner_done (Panner * self );

#line 87 "src/camera.c"
PannerList * pannerlist_new (Point goal , float speed , int immediate );

#line 96 "src/camera.c"
PannerList * pannerlist_free (PannerList * self );

#line 106 "src/camera.c"
int tracker_setflag (Tracker * self , int flag );

#line 111 "src/camera.c"
int tracker_unsetflag (Tracker * self , int flag );

#line 118 "src/camera.c"
int tracker_flag_ (Tracker * self , int flag , int set );

#line 123 "src/camera.c"
int tracker_flag (Tracker * self , int flag );

#line 128 "src/camera.c"
Tracker * tracker_done (Tracker * self );

#line 138 "src/camera.c"
Tracker * tracker_free (Tracker * self );

#line 145 "src/camera.c"
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
Tracker * tracker_alloc PROTO ((void));

#line 151 "src/camera.c"
Tracker * tracker_init (Tracker * self , Camera * camera , void * target , TrackerTrack * track );

#line 162 "src/camera.c"
Tracker * tracker_new (Camera * camera , void * data , TrackerTrack * track );

#line 167 "src/camera.c"
int tracker_apply (Tracker * self , void * data );

#line 173 "src/camera.c"
int camera_setflag (Camera * self , int flag );

#line 178 "src/camera.c"
int camera_unsetflag (Camera * self , int flag );

#line 185 "src/camera.c"
int camera_flag_ (Camera * self , int flag , int set );

#line 190 "src/camera.c"
int camera_flag (Camera * self , int flag );

#line 194 "src/camera.c"
Camera * camera_alloc PROTO ((void));

#line 199 "src/camera.c"
int camera_maxtrackers (Camera * camera );

#line 204 "src/camera.c"
Tracker * camera_tracker (Camera * camera , int index );

#line 212 "src/camera.c"
Tracker * camera_tracker_ (Camera * camera , int index , Tracker * tracker );

#line 220 "src/camera.c"
Camera * camera_freetrackers (Camera * self );

#line 234 "src/camera.c"
Camera * camera_cleartrackers (Camera * self );

#line 244 "src/camera.c"
Camera * camera_done (Camera * self );

#line 250 "src/camera.c"
Camera * camera_free (Camera * self );

#line 258 "src/camera.c"
Camera * camera_init (Camera * self , float x , float y , float w , float h );

#line 272 "src/camera.c"
Camera * camera_new (float x , float y , float w , float h );

#line 278 "src/camera.c"
int camera_applytrackers (Camera * self );

#line 293 "src/camera.c"
int camera_applypanners (Camera * self );

#line 344 "src/camera.c"
Camera * camera_update (Camera * self );

#line 353 "src/camera.c"
Point camera_at (Camera * self );

#line 358 "src/camera.c"
Point camera_at_x_ (Camera * self , float x );

#line 364 "src/camera.c"
Point camera_at_y_ (Camera * self , float y );

#line 371 "src/camera.c"
Point camera_at_xy_ (Camera * self , float x , float y );

#line 378 "src/camera.c"
Point camera_at_ (Camera * self , Point at );

#line 385 "src/camera.c"
float camera_at_x (Camera * self );

#line 390 "src/camera.c"
float camera_at_y (Camera * self );

#line 395 "src/camera.c"
float camera_w (Camera * self );

#line 400 "src/camera.c"
float camera_h (Camera * self );

#line 405 "src/camera.c"
float camera_br_x (Camera * self );

#line 410 "src/camera.c"
float camera_br_y (Camera * self );

#line 415 "src/camera.c"
float camera_center_x (Camera * self );

#line 420 "src/camera.c"
float camera_center_y (Camera * self );

#line 425 "src/camera.c"
Point camera_center (Camera * self );

#line 433 "src/camera.c"
Point camera_center_ (Camera * self , Point center );

#line 442 "src/camera.c"
Point camera_centerdelta_ (Camera * self , Point newcenter , float delta );

#line 455 "src/camera.c"
Point camera_speed_deltaxy (Camera * self , float dx , float dy );

#line 464 "src/camera.c"
Point camera_speed_xy_ (Camera * self , float x , float y );

#line 471 "src/camera.c"
Point camera_speed (Camera * self );

#line 476 "src/camera.c"
Point camera_speed_ (Camera * self , Point speed );

#line 482 "src/camera.c"
Camera * camera_debugprint (Camera * self );

#line 492 "src/camera.c"
Panner * camera_newpanner (Camera * self , Point goal , float speed , int immediate );

#line 508 "src/camera.c"
Panner * camera_freetoppanner (Camera * self );

#line 521 "src/camera.c"
void camera_freepanners (Camera * self );

#line 527 "src/camera.c"
int camera_cansee (Camera * self , int x , int y , int w , int h );

#line 537 "src/camera.c"
Tracker * camera_newtracker (Camera * self , int id , void * target , TrackerTrack * track );

#endif /* CFH_CAMERA_PROTO */
