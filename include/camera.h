#ifndef CAMERA_H
#define CAMERA_H

#include "eruta.h"
#include "inli.h"


/** Forward declaration of structs and types. */
typedef struct Camera_      Camera;
typedef struct Tracker_     Tracker;
typedef struct Panner_      Panner;  
typedef struct PannerList_  PannerList;  
typedef struct Lockin_      Lockin;  
typedef struct LockinList_  LockinList;

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








/** Panner flags. */
enum PannerFlags_ {
  PANNER_ACTIVE   = 1 << 1,
  PANNER_IMMEDIATE= 1 << 2,
};


/** Lockin Flags. */
enum LockinFlags_ {
  LOCKIN_ACTIVE = 1 << 1,
};



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


Camera * camera_done (Camera * self );
Camera * camera_free (Camera * self );
Camera * camera_init (Camera * self , float x , float y , float w , float h );
Camera * camera_new (float x , float y , float w , float h );

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
Point camera_br(Camera * self);

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

Point camera_worldtoscreen(Camera * self, Point world_pos);
Point camera_screentoworld(Camera * self, Point screen_pos);


#endif
