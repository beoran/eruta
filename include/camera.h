#ifndef CAMERA_H
#define CAMERA_H

#include "eruta.h"
#include "inli.h"
#include "rebox.h"


/** Forward declaration of structs and types. */
typedef struct Camera_      Camera;
typedef struct Tracker_     Tracker;
typedef struct Panner_      Panner;  
typedef struct PannerList_  PannerList;  
typedef struct Lockin_      Lockin;  
typedef struct LockinList_  LockinList;

#include "area.h"



/** Flags for the camera. */
enum CameraFlags_ {
  /* Camera cannot move at all. */
  CAMERA_LOCK       = 1 << 1,
  /* Camera panning disabled. */
  CAMERA_NOPAN      = 1 << 2,
  /* Camera tracking disabled */
  CAMERA_NOTRACK    = 1 << 3,
  /* Camera lockin disabled. */
  CAMERA_NOLOCKIN   = 1 << 4,
  /* Camera tracking of tracked thing is immediate and locked on to tracked thing. 
  * Otherwise, the tracking is only applied gradually.
  */
  CAMERA_TRACKLOCK  = 1 << 5  
};

/** A Panner is a goal for panning the Camera. */
struct Panner_ {
  Point goal;
  float speed;
  int   flags;
};


/** A PannerList is an intrusive linked list of panners. */
struct PannerList_ {
  Panner panner;
  Inli   list;
};

/** Panner flags. */
enum PannerFlags_ {
  PANNER_ACTIVE   = 1 << 1,
  PANNER_IMMEDIATE= 1 << 2,
};

/** A Lockin locks in the Camera in a certain rectangular region.   */
struct Lockin_ {
  Rebox box;
  int   flags;
};

/** A LockinList is an intrusive list of Lockins.  */
struct LockinList_ {
  Lockin lockin;
  Inli   list;
};

/** Lockin Flags. */
enum LockinFlags_ {
  LOCKIN_ACTIVE = 1 << 1,
};


/** Panners a camera can have. */
#define CAMERA_PANNERS 32

/** Trackers a camera can have. */
#define CAMERA_TRACKERS 32 

/** The Camera is one (or more if using split screen) of the
* rectangular views that the player has on the game world.
**/
struct Camera_ {
  Rebox        box;
  Point        speed;
  /* The "walls" the camera has to stay inside if enabled.  */
  Point        walls;
  Point        wallsize;
  /* Head of doubly linked list for the panners. */
  PannerList * panners;
  /* Head of doubly linked list for the lockins. */  
  LockinList * lockins;
  /* Thing that is being tracked. */
  Thing      * track; 
  int          flags;
};


/* This file was generated with:
'cfunctions -c -aoff -n -w camera_proto src/camera.c' */
#ifndef CFH_CAMERA_PROTO
#define CFH_CAMERA_PROTO

/* From 'src/camera.c': */

int panner_setflag (Panner * self , int flag );

int panner_unsetflag (Panner * self , int flag );

int panner_flag_ (Panner * self , int flag , int set );

int panner_flag (Panner * self , int flag );

Panner * panner_init (Panner * self , Point goal , float speed , int immediate );

Panner * panner_done (Panner * self );

PannerList * pannerlist_new (Point goal , float speed , int immediate );

PannerList * pannerlist_free (PannerList * self );

PannerList * pannerlist_freeall (PannerList * self );

Lockin * lockin_init (Lockin * self , float x , float y , float w , float h );

Lockin * lockin_done (Lockin * self );

LockinList * lockinlist_new (float x , float y , float w , float h );

LockinList * lockinlist_free (LockinList * self );

LockinList * lockinlist_freeall (LockinList * self );

int camera_setflag (Camera * self , int flag );

int camera_unsetflag (Camera * self , int flag );

int camera_flag_ (Camera * self , int flag , int set );

int camera_flag (Camera * self , int flag );

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
Camera * camera_alloc PROTO ((void));

Camera * camera_done (Camera * self );

Camera * camera_free (Camera * self );

Camera * camera_init (Camera * self , float x , float y , float w , float h );

Camera * camera_new (float x , float y , float w , float h );

Thing * camera_track_ (Camera * self , Thing * track );

int camera_panning_p (Camera * self );

int camera_lockin_p (Camera * self );

int camera_tracking_p (Camera * self );

int camera_applylockedtracking (Camera *self );

int camera_applynormaltracking (Camera *self );

int camera_applytracking (Camera *self );

int camera_applypanners (Camera * self );

int camera_applylockins (Camera * self );

Camera * camera_update (Camera * self );

Point camera_at (Camera * self );

Point camera_at_x_ (Camera * self , float x );

Point camera_at_y_ (Camera * self , float y );

Point camera_at_xy_ (Camera * self , float x , float y );

Point camera_at_ (Camera * self , Point at );

float camera_at_x (Camera * self );

float camera_at_y (Camera * self );

float camera_w (Camera * self );

float camera_h (Camera * self );

float camera_br_x (Camera * self );

float camera_br_y (Camera * self );

float camera_center_x (Camera * self );

float camera_center_y (Camera * self );

Point camera_center (Camera * self );

Point camera_center_ (Camera * self , Point center );

Point camera_centerdelta_ (Camera * self , Point newcenter , float deltax , float deltay );

Point camera_speed_deltaxy (Camera * self , float dx , float dy );

Point camera_speed_xy_ (Camera * self , float x , float y );

Point camera_speed (Camera * self );

Point camera_speed_ (Camera * self , Point speed );

Camera * camera_debugprint (Camera * self );

Panner * camera_newpanner (Camera * self , Point goal , float speed , int immediate );

Panner * camera_freetoppanner (Camera * self );

void camera_freepanners (Camera * self );

Lockin * camera_newlockin (Camera * self , float x , float y , float w , float h );

Lockin * camera_freetoplockin (Camera * self );

void camera_freelockins (Camera * self );

int camera_cansee (Camera * self , int x , int y , int w , int h );

#endif /* CFH_CAMERA_PROTO */

#endif
