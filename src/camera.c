#include "eruta.h"
#include "camera.h"
#include "camera_struct.h"
#include "mem.h"
#include "flags.h"

/* 

Ideas about camera tracking, panning and limiting.

Normally the camera tracks or follows a single mobile Thing.
Normally, the thing is only tracked if it goes out of
sight of the camera. However, it is also possible to lock 
on to the Thing exactly so the camera moves completely with 
it. 

It's possible to change the thing that is being tracked. 
This transition can be immediate or smooth. The speed
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
  self->goal  = bevec0();
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
  if (!self) { return NULL ; }
  inli_remove(&self->list);
  panner_done(&self->panner);
  return mem_free(self);
}

/** Frees all nodes of a PannerList */
PannerList * pannerlist_freeall(PannerList * self) {
  PannerList * index, * next;
  index = self;
  while (index) {
    Inli * nextlist = inli_next(&index->list);
    next = INLI_LISTDATA(nextlist, PannerList);
    pannerlist_free(index);
    index = next;
  }
  return NULL;
}

/** Initializes a lockin and sets it to be active.  */
Lockin * lockin_init(Lockin * self, float x, float y, float w, float h) {
  if(!self) return NULL;
  self->box   = rebox_new(x, y, w, h);
  self->flags = LOCKIN_ACTIVE;
  return self;
}

/** Cleans up a lockin after use. */
Lockin * lockin_done(Lockin * self) {
  if(!self) return NULL;
  self->flags = 0;
  return self;
}


/** Allocates a new lockin list node. */
LockinList * lockinlist_new(float x, float y, float w, float h) {
  LockinList * self = STRUCT_ALLOC(LockinList);
  lockin_init(&self->lockin, x, y, w, h);
  inli_init(&self->list);
  return self;
}

/** Frees a lockin list node, also removes it from 
the intrusive list it was in if that was the case.*/ 
LockinList * lockinlist_free(LockinList * self) {
  if (!self) { return NULL; }
  inli_remove(&self->list);
  lockin_done(&self->lockin);
  return mem_free(self);
}

/** Frees all nodes of a LockinList */
LockinList * lockinlist_freeall(LockinList * self) {
  LockinList * index, * next;
  index = self;
  while (index) {
    Inli * nextlist = inli_next(&index->list);
    next = INLI_LISTDATA(nextlist, LockinList);
    lockinlist_free(index);
    index = next;
  }
  return NULL;
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


/** Cleans up a camera. */
Camera * camera_done(Camera * self) {
  if(!self) { return NULL; } 
  camera_freelockins(self);
  camera_freepanners(self);
  return self;
}

/** Frees the camera after use. */
Camera * camera_free(Camera * self) {
  camera_done(self);
  STRUCT_FREE(self);
  return NULL;
}


/** Initializes a camera. */
Camera * camera_init(Camera * self, float x, float y, float w, float h) {
  if(!self) return NULL;
  self->box     = rebox_new(x, y, w, h);
  self->speed.x = 0;
  self->speed.y = 0;
  self->panners = NULL;
  self->lockins = NULL;
  self->track   = NULL;
  return self;
}

/** Alocates a new camera. */
Camera * camera_new(float x, float y, float w, float h) {
  Camera * self = camera_alloc();
  return camera_init(self, x, y, w, h);
}

/** Sets the object to be tracked. 
This object is not owned by camera. Pass in NULL to disable tracking. */
Thing * camera_track_(Camera * self, Thing * track) {
  if(!self) return NULL;
  self->track = track;
  if (self->track) {
    camera_unsetflag(self, CAMERA_NOPAN);
  } else {
    camera_setflag(self, CAMERA_NOPAN);
  }
  return self->track;
}

/** Returns nonzero if the camera is panning,
false if not */
int camera_panning_p(Camera * self) {
  Panner * panner;
  if(!self) return FALSE;
  if(!self->panners) return FALSE;
  if(camera_flag(self, CAMERA_NOPAN)) return FALSE;
  return TRUE;
}

/** Retuns nonzero if the camera is limited by lockins, 
false if not. */
int camera_lockin_p(Camera * self) {
  if(!self) return FALSE;
  if(!self->lockins) return FALSE;
  if(camera_flag(self, CAMERA_NOLOCKIN)) return FALSE;
  return TRUE;
}

/** Returns nonzero if the camera must track a Thing, zero if not. */
int camera_tracking_p(Camera * self) {
  if(!self) return FALSE;
  if(!self->track) return FALSE;
  if(camera_flag(self, CAMERA_NOTRACK)) return FALSE;
  return TRUE;
}

/* Lock on tracking*/
int camera_applylockedtracking(Camera *self) {
  // TODO: correct with half width and half height
  camera_center_(self, thing_p(self->track));
  return 1;
}

#define CAMERA_TRACK_RATIO  0.8

/* Regular tracking */
int camera_applynormaltracking(Camera *self) {
  double tracklimitx, tracklimity; 
  Thing * thing = self->track;
  tracklimitx   = camera_w(self) * CAMERA_TRACK_RATIO;  
  tracklimity   = camera_h(self) * CAMERA_TRACK_RATIO;
  // TODO: correct with half width and half height
  camera_centerdelta_(self, thing_p(thing), tracklimitx, tracklimity);
  return 0;
}

/* Tracks the active object. */
int camera_applytracking(Camera *self) {
  if(!camera_tracking_p(self)) return -1;
  if(camera_flag(self, CAMERA_TRACKLOCK)) { 
    return camera_applylockedtracking(self);
  }
  return camera_applynormaltracking(self);
}

/** Applies the currently active panner to the camera. 
*/
int camera_applypanners(Camera * self) {
  Point delta;
  Point vspeed;
  Panner * panner;
  float speed, length;
  float ratio;
  if(!camera_panning_p(self)) return -1;
  /* Immediate panning. */
  panner = &self->panners->panner;
  if (panner_flag(panner, PANNER_IMMEDIATE)) {
    camera_center_(self, panner->goal);
    camera_freetoppanner(self);
    return 0;
  }
  /**/
  
  delta = bevec_sub(camera_center(self), panner->goal);
  /* Delta has the OPPOSITE direction or angle in which 
  the camera has to scroll, however, normally delta will be 
  bigger than the speed of the panning. We only have to move 
  a fraction of that delta, at least as long 
  as the length of delta is bigger than the speed */
  length      = bevec_length(delta);
  if (length < 1.0) {
    /* Less than one pixel to move, the goal has been reached. */
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
  OPPOSITE of the delta vector, so that is why the magnitude 
  is the negative of the needed speed. */
  vspeed      = bevec_forangle(bevec_toangle(delta)); 
  self->speed = bevec_mult(vspeed, -speed); 
  return 1;  
}

/* Applies the current active lockin to the camera. */
int camera_applylockins(Camera * self) {
  Lockin * lockin;
  double dx, dy;
  Point delta;
  if(!camera_lockin_p(self)) return -1;
  lockin = &self->lockins->lockin;
  if(rebox_inside_p(&lockin->box, &self->box)) {
      /* The camera is inside the lockin box, so it's ok. */
    return -1;
  }
  dx          = rebox_delta_x(&lockin->box, &self->box);
  dy          = rebox_delta_y(&lockin->box, &self->box);
  delta       = bevec(dx, dy);
  self->box.at= bevec_add(self->box.at, delta);
  if(dx != 0.0) self->speed.x = 0.0;
  if(dy != 0.0) self->speed.y = 0.0; 
  return 0;
}

/** Updates the camera. */
Camera * camera_update(Camera * self) {
  /* Apply the camera's panners, and only track if no panning occurred. 
  Obviously, tracking and panning should be mutually exclusive. */
  if(camera_applypanners(self) <= 0) {
    camera_applytracking(self);
  }
  /* Apply the lockins, that limit the camera's motion. */
  camera_applylockins(self);
  /* Finally move at the set speed. */
  camera_at_(self, bevec_add(camera_at(self), self->speed));
  return self;
}

Rebox * camera_rebox(Camera * self) {
 return &self->box;
}

/** Return position of camera top left corner. */
Point camera_at(Camera * self) {
  return rebox_at(camera_rebox(self));
}

/** Return position of camera bottom top left corner. */
Point camera_br(Camera * self) {
  return rebox_br(camera_rebox(self));
}

/** Sets position by individual components. */
Point camera_at_x_(Camera * self, float x) {
  return rebox_x_(camera_rebox(self), x);
}

/** Sets position by individual components. */
Point camera_at_y_(Camera * self, float y) {
  return rebox_y_(camera_rebox(self), y);
}


/** Sets position by individual components. */
Point camera_at_xy_(Camera * self, float x, float y) {
  return rebox_xy_(camera_rebox(self), x, y);
}

/** Sets position. */
Point camera_at_(Camera * self, Point at) {
  return rebox_at_(camera_rebox(self), at);
}


/** Return x position of camera top left corner. */
float camera_at_x(Camera * self) {
  return rebox_x(camera_rebox(self));
}


/** Return y position of camera top left corner. */
float camera_at_y(Camera * self) {
  return rebox_y(camera_rebox(self));
}

/** Return width of camera view. */
float camera_w(Camera * self) {
  return rebox_w(camera_rebox(self));
}

/** Return height of camera view. */
float camera_h(Camera * self) {
  return rebox_h(camera_rebox(self));
}

/** Return x position of camera bottom right corner. */
float camera_br_x(Camera * self) {
  return rebox_br_x(camera_rebox(self));
}

/** Return y position of camera bottom right corner. */
float camera_br_y(Camera * self) {
  return rebox_br_y(camera_rebox(self));
}

/** Return x position of camera view center. */
float camera_center_x(Camera * self) {
  return rebox_center_x(camera_rebox(self));
}

/** Return y position of camera bottom center */
float camera_center_y(Camera * self) {
  return rebox_center_y(camera_rebox(self));
}

/** Return position of camera view center. */
Point camera_center(Camera * self) {
  return rebox_center(camera_rebox(self));
}


/** Sets position of center of camera to center. */
Point camera_center_(Camera * self, Point center) {
  return rebox_center_(camera_rebox(self), center);
}

/** Adjusts the position of center of camera to center if new center 
nears the borders of the camer view within the given deltas. */
Point 
camera_centerdelta_(Camera * self, Point newcenter, float deltax, float deltay){
  double dx, dy, minx, miny, maxx, maxy;
  Point movecenter;
  Point movement;  
  Thing * thing   = self->track;
  double tx       = thing_cx(thing);
  double ty       = thing_cy(thing);
  
  Point oldcenter = camera_center(self);
  Point vdelta    = bevec_sub(oldcenter, newcenter);
  movecenter      = oldcenter;
  deltax          = 128.0;
  deltay          = 128.0;  
  minx            = camera_at_x(self) + deltax;
  maxx            = camera_br_x(self) - deltax;
  miny            = camera_at_y(self) + deltay;
  maxy            = camera_br_y(self) - deltay;
  
  if (tx < minx) { 
    movecenter.x  = oldcenter.x - minx + tx;  
  } else if (tx > maxx) {
    movecenter.x  = oldcenter.x + tx - maxx;  
  } 

  if (ty < miny) { 
    movecenter.y  = oldcenter.y - miny + ty;  
  } else if (ty > maxy) {
    movecenter.y  = oldcenter.y + ty - maxy;  
  }
  
  
  return camera_center_(self, movecenter);
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
        camera_at_x(self), camera_at_y(self), camera_w(self),  camera_h(self),
        camera_speed(self).x,  camera_speed(self).y);
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
  self->panners = NULL;
}

/** Adds a new lockin to the camera. Returns it, or NULL if some 
error occurred, */
Lockin * camera_newlockin(Camera * self, float x, float y, float w, float h) {
  LockinList * lockinnode;
  if(!self) return NULL;
  lockinnode = lockinlist_new(x, y, w, h);
  if(!lockinnode) return NULL;
  if(!self->lockins) {
    self->lockins = lockinnode;
  } else {
    inli_push(&self->lockins->list, &lockinnode->list);
  }
  return &lockinnode->lockin;  
} 

/** Removes the topmost lockin and frees it. 
Does nothing if no lockins are installed. 
Return new topmost lockin list or null if it was the last. */
Lockin * camera_freetoplockin(Camera * self) {
  Inli       * nextlist;
  LockinList * nextlockinlist;
  if((!self) || (!self->lockins)) return NULL;
  nextlist       = inli_next(&self->lockins->list);
  nextlockinlist = INLI_LISTDATA(nextlist, LockinList);
  lockinlist_free(self->lockins);
  self->lockins  = nextlockinlist;
  if(!self->lockins) return NULL;
  return &self->lockins->lockin;
}

/** Empty the camera's list of lockins. */
void camera_freelockins(Camera * self) {
  while(camera_freetoplockin(self));
  self->lockins = NULL;
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


/* Transforms "world" (ie area/tilemap/level) coordinates to 
 * screen/camera coordinates. 
 */
Point camera_worldtoscreen(Camera * self, Point world_pos) {
  return bevec_sub(world_pos, self->box.at);
}

/* Transforms screen/camera coordinates to "world" (ie area/tilemap/level) 
 * coordinates. 
 */
Point camera_screentoworld(Camera * self, Point screen_pos) {
  return bevec_sub(screen_pos, self->box.at);
}



