
#include <stdio.h>

#include "bumpshape.h"
#include "bad.h"

/* AABB helper functions. */

/* X coordinate of left side of box. */
double bumpaabb_left(BumpAABB * self) {
  return self->p.x - self->hs.x;
}

/* X coordinate of right side of box. */
double bumpaabb_right(BumpAABB * self) {
  return self->p.x + self->hs.x;
}

/* Y coordinate of top side of box. */
double bumpaabb_top(BumpAABB * self) {
  return self->p.y - self->hs.y;
}

/* X coordinate of bottom side of box. */
double bumpaabb_down(BumpAABB * self) {
  return self->p.y + self->hs.y;
}

/* Full width of the box */
double bumpaabb_width(BumpAABB * self) {
  return 2.0 * self->hs.x;
}

/* Full height of the box */
double bumpaabb_height(BumpAABB * self) {
  return 2.0 * self->hs.y;
}

/** Makes a new bounding box. W and H are the FULL width and height of the box. */
BumpAABB bumpaabb(double cx, double cy, double w, double h) {
  BumpAABB result = { bevec(cx, cy), bevec(w / 2.0, h / 2.0) }; 
  return result;
}

/** Makes a new bounding box with integer bounds.
 * W and H are the FULL width and height of the box.
 * And x and y are the coordinates of the top left corner. */
BumpAABB bumpaabb_make_int(int x, int y, int w, int h) {
  double hw       = ((double) w) / 2.0;
  double hh       = ((double) h) / 2.0;
  double cx       = ((double) x) + hw;
  double cy       = ((double) y) + hh;
  return bumpaabb(cx, cy,  ((double) w), ((double) h)); 
}

/* Prints a textual description of the bounds box to the file out.
 * For debugging purposes */
void bumpaabb_print(BumpAABB * box, FILE * out) {
  double x = bumpaabb_left(box);
  double y = bumpaabb_top(box);
  double w = bumpaabb_width(box);
  double h = bumpaabb_height(box);
  fprintf(out, "[(%lf %lf) (%lf %lf)]", x, y, w, h);
}


BumpCircle bumpcircle(double cx, double cy, double radius) {
  BumpCircle result = { bevec(cx, cy), radius};
  return result;
}

BumpCircle bumpcircle_make_int(int x, int y, int radius) {
  double cx = (double)x;
  double cy = (double)y;
  double rr = (double)radius;
  return bumpcircle(cx, cy, rr);
}

BumpSegment bumpsegment(double x1, double y1, double x2, double y2) {
  BumpSegment result = { bevec(x1, y1), bevec(x2, y2)};
  return result;
}


BumpSegment bumpsegment_make_int(int x1, int y1, int x2, int y2) {
  double dx1      = ((double) x1);
  double dy1      = ((double) y1);
  double dx2      = ((double) x2);
  double dy2      = ((double) y2);
  return bumpsegment(dx1, dy1, dx2, dy2); 
}

BumpShape bumpcircleshape(double cx, double cy, double radius) {
  BumpCircleShape aid = { BUMPSHAPE_CIRCLE, bumpcircle(cx, cy, radius) };
  BumpShape result;
  result.circle = aid;
  return result;
}


BumpShape bumpsegmentshape(double x1, double y1, double x2, double y2) {
  BumpSegmentShape aid = { BUMPSHAPE_SEGMENT, bumpsegment(x1, y1, x2, y2) };
  BumpShape result;
  result.segment = aid;
  return result;
}

  
BumpShape bumpboxshape(double cx, double cy, double w, double h) {
  BumpBoxShape aid = { BUMPSHAPE_SEGMENT, bumpaabb(cx, cy, w, h) };
  BumpShape result;
  result.box = aid;
  return result;
}

BumpShape bumperrorshape(int code) {
  BumpErrorShape aid = { BUMPSHAPE_ERROR, code };
  BumpShape result;
  result.error = aid;
  return result;
}


BumpShape bumpshape_va(int type, va_list args) {
  double cx, cy, r, x1, x2, y1, y2, w, h;
  BumpShape result;
  switch(type) {
    case BUMPSHAPE_BOX:
      x1      = va_arg(args, double);
      y1      = va_arg(args, double);
      w       = va_arg(args, double);
      h       = va_arg(args, double);
      return bumpboxshape(x1, y1, w, h);
    break;
    
    case BUMPSHAPE_CIRCLE:  
      x1      = va_arg(args, double);
      y1      = va_arg(args, double);
      r       = va_arg(args, double);
      return bumpcircleshape(x1, y1, r);
    break;
    
    case BUMPSHAPE_SEGMENT:
      x1      = va_arg(args, double);
      y1      = va_arg(args, double);
      x2      = va_arg(args, double);
      y2      = va_arg(args, double);
      return bumpsegmentshape(x1, y1, x2, y2);
    break;
     
    default: 
      return bumperrorshape(-1);
    break;
  }
}


BumpShape bumpshape(int type, ...) {
  BumpShape result;
  va_list args;
  va_start(args, type);
  result =  bumpshape_va(type, args);
  va_end(args);
  return result;
}

BeVec bumpcircle_bumpaabb_overlap_vector(BumpCircle c, BumpAABB b) {
  float close_x = bad_clampd(c.p.x, bumpaabb_left(&b), bumpaabb_right(&b));
  float close_y = bad_clampd(c.p.y, bumpaabb_top(&b), bumpaabb_down(&b));
  float dx      = c.p.x - close_x;
  float dy      = c.p.y - close_y;
  return bevec(dx, dy); /* XXX this is wrong, need c.r in there somewhere */
}


int bumpcircle_bumpaabb_overlap_p(BumpCircle c, BumpAABB b) {
  float close_x = bad_clampd(c.p.x, bumpaabb_left(&b), bumpaabb_right(&b));
  float close_y = bad_clampd(c.p.y, bumpaabb_top(&b), bumpaabb_down(&b));
  float dx      = c.p.x - close_x;
  float dy      = c.p.y - close_y;
  return (((dx*dx) + (dy*dy)) > (c.radius * c.radius));
}

int bumpcircleshape_bumpboxshape_overlap_p(BumpCircleShape c, BumpBoxShape b) {
  return bumpcircle_bumpaabb_overlap_p(c.shape, b.shape); 
}


/* Returns overlap of the bounds boxes of self and other in x direction. 
 * May be zero or negative if no overlap.
 */ 
double bumpaabb_overlap_x(BumpAABB self, BumpAABB other) {
    if (self.p.x < other.p.x) { 
      return (self.hs.x + other.hs.x)  + (self.p.x - other.p.x ); 
    } else { 
      return (other.hs.x + self.hs.x)  + (other.p.x - self.p.x);
    } 
}

/* Returns overlap of the bounds boxes of self and other in y direction. 
 * May be zero or negative if no overlap.
 */ 
double bumpaabb_overlap_y(BumpAABB self, BumpAABB other) {
    if (self.p.y < other.p.y) { 
      return (self.hs.y + other.hs.y) + (self.p.y - other.p.y ); 
    } else { 
      return (other.hs.y + self.hs.y) + (other.p.y - self.p.y);
    } 
}

/* Returns overlap of the bounds boxes of self and other in x direction. 
 * May be zero or negative if no overlap.
 */ 
double bumpaabb_overlap_x_raw(BumpAABB self, BumpAABB other) {
    return  (self.hs.x + other.hs.x)  + (self.p.x - other.p.x ); 
}

/* Returns overlap of the bounds boxes of self and other in y direction. 
 * May be zero or negative if no overlap.
 */ 
double bumpaabb_overlap_y_raw(BumpAABB self, BumpAABB other) {
    return (self.hs.y + other.hs.y) + (self.p.y - other.p.y ); 
}


/* Returns the overlap vector between two bounds boxes. */
BeVec bumpaabb_overlap_vector(BumpAABB self, BumpAABB other) {
 return bevec(bumpaabb_overlap_x(self, other), bumpaabb_overlap_y(self, other));
}

/* Returns whether or not there is overlap between two bounds boxes. */
int bumpaabb_overlap_p(BumpAABB self, BumpAABB other) {
 if (bumpaabb_overlap_x(self, other) < 0) return FALSE; 
 return (bumpaabb_overlap_y(self, other) > 0);
}










