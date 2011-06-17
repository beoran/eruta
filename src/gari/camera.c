/**
GariCamera simulates a virtual 2D camera, which is a viewport on a scrollable 
game map.
*/

#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"

// Wind simulation, influences most particle motions and may affect tile 
// animations.
struct GariWind_ {
  int vx; 
  int vy;
};


struct GariCamera_ {
  int x, y, w, h, xmax, ymax;
  int xclamp, yclamp; // clamping values, taking h and w into consideration.
  int speed; // scrolling speed, if any.
};



GariCamera * gari_camera_init(GariCamera * cam, int x, int y, int w, int h, 
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

GariCamera * gari_camera_make(int x, int y, int w, int h, int xmax, int ymax) {
  GariCamera * cam = GARI_ALLOCATE(GariCamera);  
  return gari_camera_init(cam, x, y, w, h, xmax, ymax);  
} 

void gari_camera_free(GariCamera * cam) {
  GARI_FREE(cam);
}

/**
* Returns X coordinate of camera. 
*/
int gari_camera_x(GariCamera * cam) {
  return cam->x;
}

/**
* Returns Y coordinate of camera. 
*/
int gari_camera_y(GariCamera * cam) {
  return cam->y;
}


/**
* Immediately moves the camera so the stop left point of it's viewport 
* is at newx, newy. Will clamp newx and newy to prevent them from going 
* out of bounds.
*/

void gari_camera_setxy(GariCamera * cam, int newx, int newy) {
  int result = TRUE;
  cam->x = GARI_CLAMP(newx, 0, cam->xclamp);
  cam->y = GARI_CLAMP(newy, 0, cam->yclamp);
}; 


/** 
* Moves the camera instantly by the given increments, but will clamp 
* any motions that would go out of range. 
*/
void gari_camera_move(GariCamera * cam, int dx, int dy) {
  int result = TRUE;
  gari_camera_setxy(cam, cam->x + dx, cam->y + dy);
}; 



