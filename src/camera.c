#include <chipmunk.h>
#include "eruta.h"
#include "camera.h"
#include "mem.h"
#include "flags.h"



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

/** Uninitializes a Trqcker. */
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

/** Frees the carera after use. */
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

/** Updates the camera. */
Camera * camera_update(Camera * self) {
  /* Apply the camera's trackers. */
  camera_applytrackers(self);
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


