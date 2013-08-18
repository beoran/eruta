#ifndef CAMERA_STRUCT_H_INCLUDED
#define CAMERA_STRUCT_H_INCLUDED

#include "rebox.h"
#include "inli.h"
#include "thing.h"

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


#endif
