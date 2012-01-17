/**
GyCamera simulates a virtual 2D camera, which is a viewport on a scrollable 
game map.
*/

#include "gy.h"

#define GY_INTERN_ONLY
#include "gy_intern.h"

// Wind simulation, influences most particle motions and may affect tile 
// animations.
struct GyWind_ {
  int vx; 
  int vy;
};


struct GyCamera_ {
  int x, y, w, h, xmax, ymax;
  int xclamp, yclamp; // clamping values, taking h and w into consideration.
  int speed; // scrolling speed, if any.
};



GyCamera * gycamera_init(GyCamera * cam, int x, int y, int w, int h, 
                 int xmax, int ymax) {
  if(!cam) { return NULL ; }
  cam->x      = x;
  cam->y      = y;
  cam->xmax   = xmax;
  cam->ymax   = ymax;
  cam->w      = w;
  cam->h      = h;
  cam->xclamp = cam->xmax - cam->w;
  cam->yclamp = cam->ymax - cam->h;  
  return cam;
};

GyCamera * gycamera_make(int x, int y, int w, int h, int xmax, int ymax) {
  GyCamera * cam = GY_ALLOCATE(GyCamera);  
  return gycamera_init(cam, x, y, w, h, xmax, ymax);  
} 

void gycamera_free(GyCamera * cam) {
  GY_FREE(cam);
}

/**
* Returns X coordinate of camera. 
*/
int gycamera_x(GyCamera * cam) {
  return cam->x;
}

/**
* Returns Y coordinate of camera. 
*/
int gycamera_y(GyCamera * cam) {
  return cam->y;
}


/**
* Immediately moves the camera so the stop left point of it's viewport 
* is at newx, newy. Will clamp newx and newy to prevent them from going 
* out of bounds.
*/

void gycamera_setxy(GyCamera * cam, int newx, int newy) {
  int result = TRUE;
  cam->x = GY_CLAMP(newx, 0, cam->xclamp);
  cam->y = GY_CLAMP(newy, 0, cam->yclamp);
}; 


/** 
* Moves the camera instantly by the given increments, but will clamp 
* any motions that would go out of range. 
*/
void gycamera_move(GyCamera * cam, int dx, int dy) {
  int result = TRUE;
  gycamera_setxy(cam, cam->x + dx, cam->y + dy);
}; 



