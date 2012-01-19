#include <chipmunk.h>
#include "eruta.h"
#include "camera.h"
#include "mem.h"

/**
* The Camera is one  (or moreif using split screen) of the
* rectangular view that the player has on the game world.
*/
struct Camera_ {
  Point at;
  Point size;
  Point speed;
};

Camera * camera_alloc() {
  return STRUCT_ALLOC(Camera);
}

/** Frees the carera after use. */
Camera * camera_free(Camera * self) {
  STRUCT_FREE(self);
  return NULL;
}

Camera * camera_init(Camera * self, float x, float y, float w, float h) {
  if(!self) return NULL;
  self->at.x = x;
  self->at.y = y;
  self->size.x  = w;
  self->size.y  = h;
  self->speed.x = 0;
  self->speed.y = 0;
  return self;
}

/** Alocates a new camera. */
Camera * camera_new(float x, float y, float w, float h) {
  Camera * self = camera_alloc();
  return camera_init(self, x, y, w, h);
}

/** Updates the camera. */
Camera * camera_update(Camera * self) {
  self->at = cpvadd(self->at, self->speed);
  return self;
}

/** Return position of camera top left corner. */
Point camera_at(Camera * self) {
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









