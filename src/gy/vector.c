#include "gy.h"

#define GY_INTERN_ONLY
#include "gy_intern.h"

#include <math.h>


GyVector gyvector(GyFloat x, GyFloat y) {
  GyVector vec = { x, y } ; 
  return vec;
}

GyVector * gyvector_init(GyVector * vec, GyFloat x, GyFloat y) {
  if (!vec) return NULL;
  vec->x = x;
  vec->y = y; 
  return vec;
}

/** Allocates and initializes a Gyvectory and returns it. Retuns NULL if out 
   of memory. */
GyVector * gyvector_new(GyFloat x, GyFloat y) {
  GyVector * vec = GY_ALLOCATE(GyVector);  
  return gyvector_init(vec, x, y);
}

/** Allocates and initializes a Gyvectory and returns it. Retuns NULL if out 
   of memory. */
GyVector * gyvector_newvector(GyVector v1) {
  return gyvector_new(v1.x, v1.y);
}


/** Frees a GyVector's allocated memory. Returns NULL. */
GyVector * gyvector_free(GyVector * vec) {
  GY_FREE(vec);  
  return NULL;
}

/** Compares two floats. Retuns 0 if the floats are within delta 
 of each other, returns -1 if f1 is smaller than f2, 
 returns 1 if f1 is greater than f2. */
int gyfloat_cmp(GyFloat f1, GyFloat f2, GyFloat delta) {
  if ((f1 + delta) < f2) return -1;
  if (f1           > (f2+delta)) return -1;
  return 0;
}

/** Compares two floats. Returns TRUE if the floats are within delta 
of each other, otherwise return 0 */ 
int gyfloat_equal(GyFloat f1, GyFloat f2, GyFloat delta) {
  if ((f1 + delta) < f2) return FALSE;
  if (f1           > (f2+delta)) return FALSE;
  return TRUE;
}


/** Returns the square root of a GyFloat. */
GyFloat gyfloat_sqrt(GyFloat f1) {
  return sqrtf(f1);
}

/** Returns the sine of a GyFloat. */
GyFloat gyfloat_sin(GyFloat f1) {
  return sinf(f1);
}

/** Returns the cosine of a GyFloat. */
GyFloat gyfloat_cos(GyFloat f1) {
  return cosf(f1);
}

/** Returns the tangens of a GyFloat. */
GyFloat gyfloat_tan(GyFloat f1) {
  return tanf(f1);
}

/** Returns the arc sine of a GyFloat. */
GyFloat gyfloat_asin(GyFloat f1) {
  return asinf(f1);
}

/** Returns the arc cosine of a GyFloat. */
GyFloat gyfloat_acos(GyFloat f1) {
  return acosf(f1);
}

/** Returns the arc tangens of a GyFloat. */
GyFloat gyfloat_atan(GyFloat f1) {
  return atanf(f1);
}

/** Returns the arc tangens of y / x . */
GyFloat gyfloat_atan2(GyFloat y, GyFloat x) {
  return atan2f(y, x);
}

/** Checks if a vector is zero. Returns nonzero if it is, zero if not. */
int gyvector_zero_p(const GyVector v1) {
  return (v1.x == 0.00) && (v1.y == 0.0);
}

/** Returns a zero vector. */
GyVector gyvector_zero() {
  return gyvector(0.0, 0.0);
}

/** Compares two vectors and sees if they are within delta of each other. */
int gyvector_equal_delta(const GyVector v1, const GyVector v2, 
                            GyFloat delta) {
  if (gyvector_equal(v1, v2)) return TRUE;
  if(!gyfloat_equal(v1.x, v2.x, delta)) return FALSE;
  return (gyfloat_equal(v1.y, v2.y, delta));
}

/** Compares two vectors for strict equality. */
int gyvector_equal(const GyVector v1, const GyVector v2) {
  return ((v1.x == v2.x) && (v1.y == v2.y));
}


/** Vector addition. */
GyVector gyvector_add(GyVector v1, GyVector v2) {
  return gyvector(v1.x + v2.x , v1.y + v2.y);
}

/** Vector substraction. */
GyVector gyvector_sub(GyVector v1, GyVector v2) {
  return gyvector(v1.x - v2.x , v1.y - v2.y);
}

/** Vector negation. */
GyVector gyvector_neg(GyVector v1) {
  return gyvector(-v1.x, -v1.y);
}

/** Scalar Vector multiplication. */
GyVector gyvector_mul(GyVector v1, GyFloat f) {
  return gyvector(f * v1.x , f * v1.y);
}

/** Scalar Vector division. */
GyVector gyvector_div(GyVector v1, GyFloat f) {
  return gyvector(v1.x / f, v1.y / f);
}

/** Vector dot product. */
GyFloat gyvector_dot(GyVector v1, GyVector v2) {
  return (v1.x * v2.x + v1.y * v2.y);
}

/** 
Cross product magnitude. The cross product of 2D x,y vectors is 
a 3D vector with a z component, so this function returns only the magnitude of
that z component.  
*/
GyFloat gyvector_cross(GyVector v1, GyVector v2) {
  return v1.x*v2.y - v1.y*v2.x;
}

/** Perpendicular vector, rotated by 90 degrees, anticlockwise. */
GyVector gyvector_perp(GyVector v1) {
  return gyvector(-v1.y, v1.x);
}

/** Perpendicular vector, rotated by -90 degrees, clockwise. */
GyVector gyvector_nperp(GyVector v1) {
  return gyvector(v1.y, -v1.x);
}

/** Returns the dot poduct of the vector and itself. */
GyFloat gyvector_dotself(GyVector v1) {
  return (v1.x * v1.x + v1.y * v1.y); 
}

/** Returns the squared length of the vector. Same as dotself, really. 
Useful for comparing lengths when speed is of importance.  */
GyFloat gyvector_lensq(GyVector v1) {
  return (v1.x * v1.x + v1.y * v1.y); 
}

/** Returns the length of the vector. */
GyFloat gyvector_len(GyVector v1) {
  return gyfloat_sqrt(v1.x * v1.x + v1.y * v1.y); 
}

/** Returns a vector that is the projection of v1 onto v2*/
GyVector gyvector_project(GyVector v1, GyVector v2) {
  return gyvector_mul(v2, gyvector_dot(v1, v2) / gyvector_dotself(v2)); 
}

/** Rotates v1 by v2. */
GyVector gyvector_rotate(GyVector v1, GyVector v2) {
  return gyvector(v1.x*v2.x-v1.y*v2.y, v1.x*v2.y+v1.y*v2.x);
}  
 
/** Inverse rotation. */
GyVector gyvector_unrotate(GyVector v1, GyVector v2) {
  return gyvector(v1.x*v2.x+v1.y*v2.y, v1.x*v2.y-v1.y*v2.x);
}  

/** Linear interpolation on a line between between v1 and v2. Returns a vector 
that points to a point on the line between v1 and v2. */
GyVector gyvector_lerp(GyVector v1, GyVector v2, GyFloat tx) {
  return gyvector_add(gyvector_mul(v1, 1.0f - tx), 
                         gyvector_mul(v2, tx));
}  

/** Returns v1 normalized. Not safe for division by 0 in case length is 0. */
GyVector gyvector_normalize_unsafe(GyVector v1) {
  GyFloat len = gyvector_len(v1);  
  return gyvector(v1.x / len, v1.y / len);
}


/** Returns v1 normalized. Safe for division by 0 in case length is 0. */
GyVector gyvector_normalize(GyVector v1) {
  if(v1.x == 0.0f && v1.y == 0.0f) return gyvector(0.0f, 0.0f);
  return gyvector_normalize_unsafe(v1);  
}

/** If the length of the vector is greater than max, this function 
returns a vector with the same direction as v1, but with the length 
of that vector limited to max. Otherwise, it returns v1  */
GyVector gyvector_max(GyVector v1, GyFloat max) {
  GyFloat len = gyvector_len(v1);
  if (len <= max) return v1;
  return  gyvector_mul(gyvector_normalize(v1), max);
}

/** Returns the distance between v1 and v2. */
GyFloat gyvector_dist(GyVector v1, GyVector v2) {
  return gyvector_len(gyvector_sub(v1, v2));
}

/** Returns the squared distance between v1 and v2. */
GyFloat gyvector_distsq(GyVector v1, GyVector v2) {
  return gyvector_lensq(gyvector_sub(v1, v2));
}

/** Returns true if the distance between v1 and v2 is less than or 
equal to d. */
int gyvector_near(GyVector v1, GyVector v2, GyFloat d) {
  return gyvector_distsq(v1, v2) <= d*d;
}

/** Retuns a unit vector that makes an angle a with the X axis. */
GyVector gyvector_fromangle(GyFloat f) {
  return gyvector(gyfloat_cos(f), gyfloat_sin(f));
} 

/** Returns the angle of the vector with the X axis in clockwise 
direction in radians */
GyFloat gyvector_angle(GyVector v1) {
  return gyfloat_atan2(v1.y, v1.x);
}



