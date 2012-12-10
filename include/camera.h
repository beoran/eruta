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


#include "camera_proto.h"

#endif
