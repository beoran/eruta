#ifndef CAMERA_H
#define CAMERA_H

#include "eruta.h"

typedef struct Camera_ Camera;
typedef struct Tracker_ Tracker;


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

/** Trackers a camera can have. */
#define CAMERA_TRACKERS 32 

/** The Camera is one (or more if using split screen) of the
* rectangular views that the player has on the game world.
**/
struct Camera_ {
  Point     at;
  Point     size;
  Point     speed;
  Tracker * trackers[CAMERA_TRACKERS];
};


#include "camera_proto.h"

#endif
