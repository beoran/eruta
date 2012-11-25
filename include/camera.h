#ifndef CAMERA_H
#define CAMERA_H

#include "eruta.h"
#include "inli.h"

typedef struct Camera_ Camera;
typedef struct Tracker_ Tracker;
typedef struct Panner_ Panner;  
typedef struct PannerList_ PannerList;  


/* The tracking system is too general case, replace with 
more specific panning and trackinng functions. */


/** Flags for the camera */
enum CameraFlags_ {
  CAMERA_LOCK     = 1 << 1,
  CAMERA_PAN      = 1 << 2,
  CAMERA_TRACK    = 1 << 3,
  CAMERA_NOWALLS  = 1 << 4,
  CAMERA_NONLIMIT = 1 << 5,
  CAMERA_NOELIMIT = 1 << 6,
  CAMERA_NOSLIMIT = 1 << 7,
  CAMERA_NOWLIMIT = 1 << 8,
  CAMERA_NOLIMIT  = CAMERA_NONLIMIT | CAMERA_NOELIMIT 
                  | CAMERA_NOSLIMIT | CAMERA_NOWLIMIT,
};

/** A Panner is a goal for panning. */
struct Panner_ {
  Point goal;
  float speed;
  int   flags;
};


/** A PannerNode is an intrusive linked list of panners. */
struct PannerList_ {
  Panner panner;
  Inli   list;
};

/** Panner flags. */
enum PannerFlags_ {
  PANNER_ACTIVE   = 1 << 1,
  PANNER_IMMEDIATE= 1 << 2,
};


/** Tracking function. Should return one of the constants below to influence tracking. */
typedef int TrackerTrack(Tracker * tracker, void * extra);

/* Trackers are run one after the other. */

/* Somegthing went wrong. Apply next tracker anyway. */
#define TRACKER_ERROR    -1 
/* OK, apply next tracker. */
#define TRACKER_OK        1 
/* Tracking, ok ignore all further trackers. */
#define TRACKER_DONE      2

/** Tracker flags */
#define TRACKER_ENABLED   1
#define TRACKER_ENABLE    1
/** Tracker flags */
#define TRACKER_UNUSED    2

/** Tracker ID's. */
/** IDs for goal based trackers. */
#define TRACKER_GOAL      5 
#define TRACKER_GOAL_2    6 
#define TRACKER_GOAL_3    7
/** ID's for PC trackers. */
#define TRACKER_PC       15 
#define TRACKER_PC_2     16
#define TRACKER_PC_3     17



/** 
A tracker is an interface that helps the camera track or follow 
a certain thing or certain directions.
*/
struct Tracker_ {
  void          * target; /* Object to track */
  Camera        * camera; /* Camera to influence. */
  int             flags;  /* Tracker flags. */
  TrackerTrack  * track;
};

/** Panners a camera can have. */
#define CAMERA_PANNERS 32

/** Trackers a camera can have. */
#define CAMERA_TRACKERS 32 

/** The Camera is one (or more if using split screen) of the
* rectangular views that the player has on the game world.
**/
struct Camera_ {
  Point        at;
  Point        size;
  Point        speed;
  /* Doubly linked list for the panners. */
  PannerNode * panners; 
  Tracker    * trackers[CAMERA_TRACKERS];
  int          flags;
};


#include "camera_proto.h"

#endif
