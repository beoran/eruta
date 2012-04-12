#ifndef CAMERA_PROTO_H
#define CAMERA_PROTO_H
/*
This file was autogenerated from src/camera.c
by bin/genproto
Please do not hand edit.
*/

/**
* The Camera is one  (or more if using split screen) of the
* rectangular views that the player has on the game world.
*/
struct Camera_;
typedef struct Camera_ Camera;

/** Frees the carera after use. */
Camera * camera_free(Camera * self);

/** Alocates a new camera. */
Camera * camera_new(float x, float y, float w, float h);

/** Updates the camera. */
Camera * camera_update(Camera * self);

/** Return position of camera top left corner. */
Point camera_at(Camera * self);

/** Sets position by individual components. */
Point camera_at_xy_(Camera * self, float x, float y);

/** Sets position. */
Point camera_at_(Camera * self, Point at);

/** Return x position of camera top left corner. */
float camera_at_x(Camera * self);

/** Return y position of camera top left corner. */
float camera_at_y(Camera * self);

/** Return width of camera view. */
float camera_w(Camera * self);

/** Return height of camera view. */
float camera_h(Camera * self);

/** Return x position of camera bottom right corner. */
float camera_br_x(Camera * self);

/** Return y position of camera bottom right corner. */
float camera_br_y(Camera * self);

/** Return x position of camera view center. */
float camera_center_x(Camera * self);

/** Return y position of camera bottom center */
float camera_center_y(Camera * self);

/** Modifies speed by individual components. */
Point camera_speed_deltaxy(Camera * self, float dx, float dy);

/** Sets speed by individual components. */
Point camera_speed_xy_(Camera * self, float x, float y);

/**  Gets speed. */
Point camera_speed(Camera * self);  

/** Sets speed.  */
Point camera_speed_(Camera * self, Point speed);

/** Prints camera descriiption for debug */
Camera * camera_debugprint(Camera * self);

#endif // CAMERA_PROTO_H

