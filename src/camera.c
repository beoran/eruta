#include <chipmunk.h>
#include "eruta.h"
#include "camera.h"
#include "mem.h"
#include "flags.h"

/* 

Ideas about camera tracking, panning and limiting.

Normally the camera tracks or follows a single mobile Thing.
Normally, the ting is only tracked if it goes out of
sight of the camera. However, it is also possible to lock 
on to the Thing exactly so the camera moves completely with 
it. 

It's possible to change the thing that is being tracked. 
This transitiion can be immediate or smooth. The speed
of this transition can be set.

Sometimes the camera pans around. It moves it's center between 
a list of goal points. This motion can be immediate or smooth.
The speed of this transition may be set. Panning has priority
over tracking. It may be a good idea to implement
the transition between two things as immediate, but set up a panning
from the old thing's position to the new thing's one. 

On top of that, the motion of the camera is limited by 
the limits of the current tile map, and possibly
by special tiles that prevent the camera to pan or 
scroll or track past them. 

These map limits may be disabled individually in any 4 directions,
The effect of the scroll lock tiles may be also be disbled.

Finally, the camera may be completely locked in to place. 
This could be used, for example, in battle mode.

*/

/** Sets an individual flag on the Panner. */
int panner_setflag(Panner * self, int flag) {
  return flags_set(&self->flags, flag);
}

/** Unsets an individual flag on the Panner. */
int panner_unsetflag(Panner * self, int flag) {
  register int wflags = self->flags;
  return flags_unset(&self->flags, flag);
}

/** Sets or unsets an individual flag on the Panner. 
If set is true the flag is set, if false it's unset. */
int panner_flag_(Panner * self, int flag, int set) {
  return flags_put(&self->flags, flag, set);
}

/** Checks if an individual flag is set. */
int panner_flag(Panner * self, int flag) {
  return flags_get(self->flags, flag);
}

#define PANNER_SPEED_DEFAULT 10.0

/** Initializes a panner and sets it to be active. If speed is negative
or zero it will be replaced by 10.0 */
Panner * panner_init(Panner * self, Point goal, float speed, int immediate) {
  if(!self) return NULL;
  self->goal  = goal;
  self->speed = speed;
  if(self->speed <= 0) { self->speed = PANNER_SPEED_DEFAULT; }
  panner_flag_(self, PANNER_IMMEDIATE, immediate);
  panner_flag_(self, PANNER_ACTIVE, true);
  return self;
}

/** Cleans up a panner after use. */
Panner * panner_done(Panner * self) {
  if(!self) return NULL;
  self->goal  = cpvzero;
  self->speed = 0.0;
  return self;
}


/** Allocates a new panner list node. */
PannerList * pannerlist_new(Point goal, float speed, int immediate) {
  PannerList * self = STRUCT_ALLOC(PannerList);
  panner_init(&self->panner, goal, speed, immediate);
  inli_init(&self->list);
  return self;
}

/** Frees a panner list node, also removes it from 
the intrusive list it was in if that was the case.*/ 
PannerList * pannerlist_free(PannerList * self) {
  if (self) {
    inli_remove(&self->list);
    panner_done(&self->panner);
  }
  return mem_free(self);
}


/** Sets an individual flag on the Tracker. */
int tracker_setflag(Tracker * self, int flag) {
  return flags_set(&self->flags, flag);
}

/** Unsets an individual flag on the Tracker. */
int tracker_unsetflag(Tracker * self, int flag) {
  register int wflags = self->flags;
  return flags_unset(&self->flags, flag);
}

/** Sets or unsets an individual flag on the Tracker. 
If set is true the flag is set, if false it's unset. */
int tracker_flag_(Tracker * self, int flag, int set) {
  return flags_put(&self->flags, flag, set);
}

/** Checks if an individual flag is set. */
int tracker_flag(Tracker * self, int flag) {
  return flags_get(self->flags, flag);
}

/** Uninitializes a Tracker. */
Tracker * tracker_done(Tracker * self) {
  if(!self) return NULL;
  self->target  = NULL;
  self->track   = NULL;
  self->camera  = NULL;
  self->flags   = 0;
  return self;
}

/** Frees a Tracker. */
Tracker * tracker_free(Tracker * self) {
  tracker_done(self);
  mem_free(self);
  return NULL;
}

/** Allocates a Tracker. */
Tracker * tracker_alloc() {
  return STRUCT_ALLOC(Tracker);
}

/** Initializes a tracker */
Tracker * tracker_init(Tracker * self, Camera * camera, 
                       void * target, TrackerTrack * track) {
  if(!self) return NULL;
  self->camera = camera;
  self->target = target;
  self->track  = track;
  self->flags  = 0;
  tracker_flag_(self, TRACKER_ENABLE, 1);
  return self;  
}

/** Makes a new tracker and initializes it. */
Tracker * tracker_new(Camera * camera, void * data, TrackerTrack * track) {
  return tracker_init(tracker_alloc(), camera, data, track);
}

/** Applies a tracker. Returns the result of the track function. */
int tracker_apply(Tracker * self, void * data) {
  if((!self) | (!self->track)) return TRACKER_ERROR;
  return self->track(self, data);
} 

/** Sets an individual flag on the Camera. */
int camera_setflag(Camera * self, int flag) {
  return flags_set(&self->flags, flag);
}

/** Unsets an individual flag on the Camera. */
int camera_unsetflag(Camera * self, int flag) {
  register int wflags = self->flags;
  return flags_unset(&self->flags, flag);
}

/** Sets or unsets an individual flag on the Camera. 
If set is true the flag is set, if false it's unset. */
int camera_flag_(Camera * self, int flag, int set) {
  return flags_put(&self->flags, flag, set);
}

/** Checks if an individual flag is set. */
int camera_flag(Camera * self, int flag) {
  return flags_get(self->flags, flag);
}

Camera * camera_alloc() {
  return STRUCT_ALLOC(Camera);
}

/** Returns the amount of trackers a camera can have. */
int camera_maxtrackers(Camera * camera) {
  return CAMERA_TRACKERS;
}

/** Gets tracker at the index or NULL if not set or on error. */
Tracker * camera_tracker(Camera * camera, int index) {
  if (!camera)    return NULL;
  if (index < 0)  return NULL;
  if (index >= camera_maxtrackers(camera)) return NULL;
  return camera->trackers[index];
}

/** Sets tracker at the index and returns it, or NULL on error. */
Tracker * camera_tracker_(Camera * camera, int index, Tracker * tracker) {
  if (!camera)    return NULL;
  if (index < 0)  return NULL;
  if (index >= camera_maxtrackers(camera)) return NULL;
  return camera->trackers[index] = tracker;
}

/** Deletes all trackers of the camera. */
Camera * camera_freetrackers(Camera * self) {
  int index, stop;
  stop = camera_maxtrackers(self);
  for (index = 0; index < stop; index++) {
    Tracker * tracker = camera_tracker(self, index);
    if (tracker) { 
      tracker_free(tracker);
      camera_tracker_(self, index, NULL);      
    }
  }
  return self;
}

/** Empties all trackers of the camera. Does NOT delete them. */
Camera * camera_cleartrackers(Camera * self) {
  int index, stop;
  stop = camera_maxtrackers(self);
  for (index = 0; index < stop; index++) {
    camera_tracker_(self, index, NULL);
  }
  return self;
}

/** Cleans up a camera. */
Camera * camera_done(Camera * self) {
  camera_freetrackers(self);
  return self;
}

/** Frees the carmera after use. */
Camera * camera_free(Camera * self) {
  camera_done(self);
  STRUCT_FREE(self);
  return NULL;
}


/** Initializes a camera. */
Camera * camera_init(Camera * self, float x, float y, float w, float h) {
  if(!self) return NULL;
  self->at.x    = x;
  self->at.y    = y;
  self->size.x  = w;
  self->size.y  = h;
  self->speed.x = 0;
  self->speed.y = 0;
  self->panners = NULL;
  camera_cleartrackers(self);  
  return self;
}

/** Alocates a new camera. */
Camera * camera_new(float x, float y, float w, float h) {
  Camera * self = camera_alloc();
  return camera_init(self, x, y, w, h);
}

/** Applies all trackers to the camera */
int camera_applytrackers(Camera * self) {
  int index, stop;
  stop = camera_maxtrackers(self);
  for (index = 0; index < stop; index++) {
    Tracker * tracker = camera_tracker(self, index);
    if (tracker && tracker_flag(tracker, TRACKER_ENABLE)) { 
      int result = tracker_apply(tracker, NULL);
      if (result == TRACKER_DONE) return result;
    }
  }  
  return TRACKER_OK;
}

/* Returns nonzero if the camera is panning,
false if not */
int camera_panning_p(Camera * self) {
  Panner * panner;
  if(!self) return FALSE;
  if(!self->panners) return FALSE;
  panner = &self->panners->panner;
  if (!panner_flag(panner, PANNER_ACTIVE)) {
    return FALSE;
  }
  return TRUE;
}



/** Applies the currently active panner to the camera. 
*/
int camera_applypanners(Camera * self) {
  Point delta;
  Point vspeed;
  Panner * panner;
  cpFloat speed, length;
  cpFloat ratio;
  if(!camera_panning_p(self)) return -1;
  /* Immediate panning. */
  panner = &self->panners->panner;
  if (panner_flag(panner, PANNER_IMMEDIATE)) {
    camera_center_(self, panner->goal);
    camera_freetoppanner(self);
    return 0;
  }
  /**/
  
  delta = cpvsub(camera_center(self), panner->goal);
  /* Delta has the direction or angle in which 
  the camera has to scroll, however, normally delta will be 
  bigger than the speed of the panning. We only have to move 
  a fraction of that delta, at least as long 
  as the length of delta is bigger than the speed */
  length      = cpvlength(delta);
  if (length < 1.0) {
    /* Less than one pixel to move, the goal hss been reached. */
    camera_center_(self, panner->goal);
    camera_freetoppanner(self);
    return 0;
  }
  
  if (length < panner->speed) { 
    speed     = length;
  } else {
    speed     = panner->speed;
  }
  /* Ensure negative speed is not possible.  */
  if(speed <= 0.0) { speed = 1.0; }

  /* Construct the speed vector, it has the same angle as the 
  delta vector, but the magnitude is that of the needed speed */
  vspeed      = cpvforangle(cpvtoangle(delta)); 
  self->speed = cpvmult(vspeed, speed); 
  return 1;
  
}

/** Updates the camera. */
Camera * camera_update(Camera * self) {
  /* Apply the camera's panners. */
  // camera_applytrackers(self);
  camera_applypanners(self);
  /* Finally move at the set speed. */
  self->at = cpvadd(self->at, self->speed);
  return self;
}

/** Return position of camera top left corner. */
Point camera_at(Camera * self) {
  return self->at;
}

/** Sets position by individual components. */
Point camera_at_x_(Camera * self, float x) {
  self->at.x = x;
  return self->at;
}

/** Sets position by individual components. */
Point camera_at_y_(Camera * self, float y) {
  self->at.y = y;
  return self->at;
}


/** Sets position by individual components. */
Point camera_at_xy_(Camera * self, float x, float y) {
  self->at.x = x;
  self->at.y = y;
  return self->at;
}

/** Sets position. */
Point camera_at_(Camera * self, Point at) {
  self->at = at;
  return self->at;
}


/** Return x position of camera top left corner. */
float camera_at_x(Camera * self) {
  return self->at.x;
}

/** Return y position of camera top left corner. */
float camera_at_y(Camera * self) {
  return self->at.y;
}

/** Return width of camera view. */
float camera_w(Camera * self) {
  return self->size.x;
}

/** Return height of camera view. */
float camera_h(Camera * self) {
  return self->size.y;
}

/** Return x position of camera bottom right corner. */
float camera_br_x(Camera * self) {
  return self->at.x + self->size.x;
}

/** Return y position of camera bottom right corner. */
float camera_br_y(Camera * self) {
  return self->at.y + self->size.y;
}

/** Return x position of camera view center. */
float camera_center_x(Camera * self) {
  return self->at.x + (self->size.x / 2);
}

/** Return y position of camera bottom center */
float camera_center_y(Camera * self) {
  return self->at.y + (self->size.y / 2);;
}

/** Return position of camera view center. */
Point camera_center(Camera * self) {
  return cpv(camera_center_x(self), camera_center_y(self));
}




/** Sets position of center of camera to center. */
Point camera_center_(Camera * self, Point center) {
  Point at;
  at.x = center.x - (camera_w(self) / 2);
  at.y = center.y - (camera_h(self) / 2);
  return camera_at_(self, at);
}

/** Sets position of center of camera to center if the distance 
of the current center is greater than the given delta . */
Point camera_centerdelta_(Camera * self, Point newcenter, float delta) {
  float dist;
  Point oldcenter = camera_center(self);
  dist = cpvdist(oldcenter, newcenter);   
  if (dist < delta) {
    return oldcenter;
  }
  return camera_center_(self, newcenter);
}



/** Modifies speed by individual components. */
Point camera_speed_deltaxy(Camera * self, float dx, float dy) {
  self->speed.x += dx;
  self->speed.y += dy;
  return self->speed;
}



/** Sets speed by individual components. */
Point camera_speed_xy_(Camera * self, float x, float y) {
  self->speed.x = x;
  self->speed.y = y;
  return self->speed;
}

/**  Gets speed. */
Point camera_speed(Camera * self) {  
  return self->speed;
}

/** Sets speed.  */
Point camera_speed_(Camera * self, Point speed) {
  self->speed = speed;
  return self->speed;
}

/** Prints camera description for debug */
Camera * camera_debugprint(Camera * self) {
  if(!self) { puts("Camera is NULL!"); return NULL; }
  printf("Camera at (%f, %f), size (%f, %f), speed(%f, %f)\n",
        self->at.x, self->at.y, self->size.x,  self->size.y,
        self->speed.x,  self->speed.y);
  return self;
}

/** Adds a new panner to the camera. Returns it, or NULL if some 
error occurred, */
Panner * camera_newpanner(Camera * self, Point goal, float speed, int immediate) {
  PannerList * pannernode;
  if(!self) return NULL;
  pannernode = pannerlist_new(goal, speed, immediate);
  if(!pannernode) return NULL;
  if(!self->panners) {
    self->panners = pannernode;
  } else {
    inli_push(&self->panners->list, &pannernode->list);
  }
  return &pannernode->panner;  
} 

/** Removes the topmost panner and frees it. 
Does nothing if no panners are installed. 
Return new topmost panner list or null if it was the last. */
Panner * camera_freetoppanner(Camera * self) {
  Inli       * nextlist;
  PannerList * nextpannerlist;
  if((!self) || (!self->panners)) return NULL;
  nextlist       = inli_next(&self->panners->list);
  nextpannerlist = INLI_LISTDATA(nextlist, PannerList);
  pannerlist_free(self->panners);
  self->panners  = nextpannerlist;
  if(!self->panners) return NULL;
  return &self->panners->panner;
}

/** Empty the camera's list of panners. */
void camera_freepanners(Camera * self) {
  while(camera_freetoppanner(self));
}

/** Returns true if an object at x, y with the given bounds w and h will 
be visible to this camera, false if not. */
int camera_cansee(Camera * self, int x, int y, int w, int h) {
  // not visible if too far to the left or bottom
  if (x     > camera_br_x(self)) return FALSE;
  if (y     > camera_br_y(self)) return FALSE;
  if ((x+w) < camera_at_x(self)) return FALSE;  
  if ((y+h) < camera_at_y(self)) return FALSE;
  return TRUE;
}

/** Adds a new tracker to the camera.  */
Tracker * camera_newtracker(Camera * self, int id, void * target, TrackerTrack * track) {
  Tracker * tracker = tracker_new(self, target, track);
  if (!tracker) return NULL;
  /* Tracker index is set by user because priority is important. */
  if(!camera_tracker_(self, id, tracker)) {
    return tracker_free(tracker);
  }
  return tracker;
} 


