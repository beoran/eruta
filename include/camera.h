#ifndef CAMERA_H
#define CAMERA_H

#include "eruta.h"
#include "inli.h"
#include "rebox.h"

typedef struct Camera_      Camera;
typedef struct Tracker_     Tracker;
typedef struct Panner_      Panner;  
typedef struct PannerList_  PannerList;  
typedef struct Lockin_      Lockin;  
typedef struct LockinList_  LockinList;




/* The tracking system is too general case, replace with 
more specific panning, locking and tracking functions. */


/** Flags for the camera */
enum CameraFlags_ {
  CAMERA_LOCK     = 1 << 1,
  CAMERA_PAN      = 1 << 2,
  CAMERA_TRACK    = 1 << 3,
  CAMERA_NOLOCKIN = 1 << 4,
  CAMERA_NONLIMIT = 1 << 5,
  CAMERA_NOELIMIT = 1 << 6,
  CAMERA_NOSLIMIT = 1 << 7,
  CAMERA_NOWLIMIT = 1 << 8,
  CAMERA_NOLIMIT  = CAMERA_NONLIMIT | CAMERA_NOELIMIT 
                  | CAMERA_NOSLIMIT | CAMERA_NOWLIMIT,
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
  int          flags;
};


#include "camera_proto.h"

#endif
