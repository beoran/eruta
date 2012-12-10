
#include "rebox.h"
#include <stdlib.h>
#include "flags.h"

/** Returns a newly initialised rectangular box. */
Rebox rebox_new(cpFloat x, cpFloat y, cpFloat w, cpFloat h) {
  Rebox result;
  /** Avoid negative widths and heights by adjusting x and y accordingly */
  if (w < 0) { x = x - w ; w = -w ; } 
  if (h < 0) { y = y - h ; w = -h ; } 
  result.at   = point(x, y);
  result.size = point(w, h);
  return result;
}


/** Return position of Rebox top left corner. */
Point rebox_at(Rebox * self) {
  return self->at;
}

/** Sets position by individual components. */
Point rebox_x_(Rebox * self, float x) {
  self->at.x = x;
  return self->at;
}

/** Sets position by individual components. */
Point rebox_y_(Rebox * self, float y) {
  self->at.y = y;
  return self->at;
}


/** Sets position by individual components. */
Point rebox_xy_(Rebox * self, float x, float y) {
  self->at.x = x;
  self->at.y = y;
  return self->at;
}

/** Sets position. */
Point rebox_at_(Rebox * self, Point at) {
  self->at = at;
  return self->at;
}

/** Return x position of Rebox top left corner. */
float rebox_x(Rebox * self) {
  return self->at.x;
}

/** Return y position of Rebox top left corner. */
float rebox_y(Rebox * self) {
  return self->at.y;
}

/** Return width of Rebox view. */
float rebox_w(Rebox * self) {
  return self->size.x;
}

/** Return height of Rebox view. */
float rebox_h(Rebox * self) {
  return self->size.y;
}

/** Return x position of Rebox bottom right corner. */
float rebox_br_x(Rebox * self) {
  return self->at.x + self->size.x;
}

/** Return y position of Rebox bottom right corner. */
float rebox_br_y(Rebox * self) {
  return self->at.y + self->size.y;
}

/** Return x position of Rebox view center. */
float rebox_center_x(Rebox * self) {
  return self->at.x + (self->size.x / 2);
}

/** Return y position of Rebox bottom center */
float rebox_center_y(Rebox * self) {
  return self->at.y + (self->size.y / 2);;
}

/** Return position of Rebox view center. */
Point rebox_center(Rebox * self) {
  return cpv(rebox_center_x(self), rebox_center_y(self));
}


/** Sets position of center of Rebox to center. */
Point rebox_center_(Rebox * self, Point center) {
  Point at;
  at.x = center.x - (rebox_w(self) / 2);
  at.y = center.y - (rebox_h(self) / 2);
  return rebox_at_(self, at);
}



/* Returns TRUE if box 2 if other is wholly inside self, FALSE if not. */
int rebox_inside_p(Rebox * self, Rebox * other) {
  if (other->at.x < self->at.x) return FALSE;
  if (other->at.y < self->at.y) return FALSE;
  if (rebox_br_x(other) > rebox_br_x(self)) return FALSE;
  if (rebox_br_y(other) > rebox_br_y(self)) return FALSE;
  return TRUE;
}

/* Returns 0 if box 2 if other is wholly inside self,
otherwise the distance the box other has to move 
in the x direction to make it so. 
Only works if other is smaller than self. */
double rebox_delta_x(Rebox * self, Rebox * other) {
  double dx = self->at.x - other->at.x; 
  if (dx > 0) return dx; 
  dx = rebox_br_x(self) - rebox_br_x(other);
  if (dx > 0) return -dx;
  return 0.0;
}

/* Returns 0 if box 2 if other is wholly inside self,
otherwise the distance the box other has to move 
in the y direction to make it so. 
Only works if other is smaller than self. */
double rebox_delta_y(Rebox * self, Rebox * other) {
  double dy = self->at.y - other->at.y; 
  if (dy > 0) return dy; 
  dy = rebox_br_y(self) - rebox_br_y(other);
  if (dy > 0) return -dy;
  return 0.0;
}

