#include <stdlib.h>
#include <math.h>

#include "gy.h"
#define GY_INTERN_ONLY
#include "gy_intern.h"


/** Initializes an affine matrix. */
GyAffine * gyaffine_init(GyAffine * self, 
           double x1, double x2, double x3, double y1, double y2, double y3) {
  if(!self) return NULL;
  self->x1 = x1;
  self->x2 = x2;
  self->x3 = x3;
  self->y1 = y1;
  self->y2 = y2;
  self->y3 = y3;
  return self;
}


/** Applies an affine matrix to a point. */
GyAffine * gyaffine_apply(GyAffine * self, double srcx, double srcy, 
               double * dstx, double * dsty ) {
  (*dstx) = srcx * self->x1  + srcy * self->x2 + self->x3;
  (*dsty) = srcx * self->y2  + srcy * self->y2 + self->y3;
  return self;
}

/** Applies an affine matrix to an integer point. */
GyAffine * gyaffine_iapply(GyAffine * self, int srcx, int srcy, 
               int * dstx, int * dsty ) {
  double rx, ry;
  gyaffine_apply(self, srcx, srcy, &rx, &ry);
  (*dstx) = (int)floor(rx + 0.5); // no round() in C89 :p
  (*dsty) = (int)floor(ry + 0.5);
  return self;
}


/** Initializes an identity affine matrix. */
GyAffine * gyaffine_initidentity(GyAffine * self) {
  return gyaffine_init(self, 1, 0, 0, 0, 1, 0);
}

/** Initializes an affine matrix for scaling. */
GyAffine * gyaffine_initscale(GyAffine * self, double sx, double sy) {
  if(!self) return NULL;
  return gyaffine_init(self, sx, 0, 0, 0, sy, 0);
}

/** Initializes an affine matrix for translation. */
GyAffine * gyaffine_inittranslate(GyAffine * self, double dx, double dy) {
  if(!self) return NULL;
  return gyaffine_init(self, 1, 0, dx, 0, 1, dy);
}

/** Initializes an affine matrix for shearing. Angle is in radians. */
GyAffine * gyaffine_initshear(GyAffine * self, double angle) {
  if(!self) return NULL;
  double sh = tan(angle);
  return gyaffine_init(self, 1, sh, 0, 0, 1, 0);
}

/** Initializes an affine matrix for rotation. Angle is in radians. */
GyAffine * gyaffine_initrotate(GyAffine * self, double angle) {
  if(!self) return NULL;
  double si = sin(angle);
  double co = cos(angle);
  return gyaffine_init(self, co, -si, 0, si, co, 0);
}



/** Affine matrix multiplication
*                           m2 
*               [                   x1                  x2                  x3 ]
*               [                   y1                  y2                  y3 ]
*    m1         [                   0                   0                    1 ]
* [ x1 x2 x3 ]  [    x1*x1+x2*y1+x3*0    x1*x2+x2*y2+x3*0     x1*x3+x2*y3+x3*1 ]
* [ y1 y2 y3 ]  [    y1*x1+y2*y1+y3*0    y1*x2+y2*y2+y3*0     y1*x3+y2*y3+y3*1 ]
* [  0  0  1 ]  [     0*x1+0*y1+x3*0      0*x2+0*y2+x3*0         0*x3+0*y3+1*1 ]
* 
*/
/**
* multiplies matrix m1 with m2 and puts the results into res,
* which should be preallocated, but which may be equal to m1 or m2. 
* Returns res on success, NULL if res was null.
*/
GyAffine * gyaffine_multiply(GyAffine * res, GyAffine * m1, GyAffine * m2) {
  if(!res) return NULL;
  double x1 = m1->x1*m2->x1 + m1->x2*m2->y1;
  double x2 = m1->x1*m2->x2 + m1->x2*m2->y2;
  double x3 = m1->x1*m2->x3 + m1->x2*m2->y3 + m1->x3;
  double y1 = m1->y1*m2->x1 + m1->y2*m2->y1;
  double y2 = m1->y1*m2->x2 + m1->y2*m2->y2;
  double y3 = m1->y1*m2->x3 + m1->y2*m2->y3 + m1->y3;
  return gyaffine_init(res, x1, x2, x3, y1, y2, y3);
}



/** Applies a scaling operation to the affine matrix self. */
GyAffine * gyaffine_scale(GyAffine * self, double sx, double sy) {
  GyAffine aid;
  gyaffine_initscale(&aid, sx, sy);
  return gyaffine_multiply(self, self, &aid); 
}


/** Applies a shearing operation to the affine matrix self. */
GyAffine * gyaffine_shear(GyAffine * self, double angle) {
  GyAffine aid;
  gyaffine_initshear(&aid, angle);
  return gyaffine_multiply(self, self, &aid); 
}

/** Applies a translation operation to the affine matrix self. */
GyAffine * gyaffine_translate(GyAffine * self, double dx, double dy) {
  GyAffine aid;
  gyaffine_inittranslate(&aid, dx, dy);
  return gyaffine_multiply(self, self, &aid); 
}

/** Applies a rotation operation to the affine matrix self. */
GyAffine * gyaffine_rotate(GyAffine * self, double angle) {
  GyAffine aid;
  gyaffine_initrotate(&aid, angle);
  return gyaffine_multiply(self, self, &aid); 
}


/** Inverts the affine matrix src into dst. Both may be the same.
* returns NULL in case the matrix cannot be inverted. 
*/
GyAffine * gyaffine_invert(GyAffine * dst, GyAffine * src) {
  double det, rdet;
  det  = (src->x1*src->x2 - src->y1*src->y2);
  if (det == 0.0) { 
    return NULL;  
  } else { 
    double x1 =  src->x2 / det;
    double y1 = -src->y1 / det;
    double x2 = -src->x2 / det;
    double y2 =  src->x1 / det;
    double x3 = -src->x3 * x1 - src->y3 * x2;
    double y3 = -src->x3 * y1 - src->y3 * y2;
    return gyaffine_init(dst, x1, x2, x3, y1, y2, y3);
  }
}





