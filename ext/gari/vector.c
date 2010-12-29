#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"

#include <math.h>


GariVector gari_vector(GariFloat x, GariFloat y) {
  GariVector vec = { x, y } ; 
  return vec;
}

GariVector * gari_vector_init(GariVector * vec, GariFloat x, GariFloat y) {
  if (!vec) return NULL;
  vec->x = x;
  vec->y = y; 
  return vec;
}

/** Allocates and initializes a Garivectory and returns it. Retuns NULL if out 
   of memory. */
GariVector * gari_vector_new(GariFloat x, GariFloat y) {
  GariVector * vec = GARI_ALLOCATE(GariVector);  
  return gari_vector_init(vec, x, y);
}

/** Allocates and initializes a Garivectory and returns it. Retuns NULL if out 
   of memory. */
GariVector * gari_vector_newvector(GariVector v1) {
  return gari_vector_new(v1.x, v1.y);
}


/** Frees a GariVector's allocated memory. Returns NULL. */
GariVector * gari_vector_free(GariVector * vec) {
  GARI_FREE(vec);  
  return NULL;
}

/** Compares two floats. Retuns 0 if the floats are within delta 
 of each other, returns -1 if f1 is smaller than f2, 
 returns 1 if f1 is greater than f2. */
int gari_float_cmp(GariFloat f1, GariFloat f2, GariFloat delta) {
  if ((f1 + delta) < f2) return -1;
  if (f1           > (f2+delta)) return -1;
  return 0;
}

/** Compares two floats. Returns TRUE if the floats are within delta 
of each other, otherwise return 0 */ 
int gari_float_equal(GariFloat f1, GariFloat f2, GariFloat delta) {
  if ((f1 + delta) < f2) return FALSE;
  if (f1           > (f2+delta)) return FALSE;
  return TRUE;
}

/** Returns the square root of a GariFloat. */
GariFloat gari_float_sqrt(GariFloat f1) {
  return sqrtf(f1);
}

/** Returns the sine of a GariFloat. */
GariFloat gari_float_sin(GariFloat f1) {
  return sinf(f1);
}

/** Returns the cosine of a GariFloat. */
GariFloat gari_float_cos(GariFloat f1) {
  return cosf(f1);
}

/** Returns the tangens of a GariFloat. */
GariFloat gari_float_tan(GariFloat f1) {
  return tanf(f1);
}

/** Returns the arc sine of a GariFloat. */
GariFloat gari_float_asin(GariFloat f1) {
  return asinf(f1);
}

/** Returns the arc cosine of a GariFloat. */
GariFloat gari_float_acos(GariFloat f1) {
  return acosf(f1);
}

/** Returns the arc tangens of a GariFloat. */
GariFloat gari_float_atan(GariFloat f1) {
  return atanf(f1);
}

/** Returns the arc tangens of y / x . */
GariFloat gari_float_atan2(GariFloat y, GariFloat x) {
  return atan2f(y, x);
}



/** Compares two vectors and sees if they are within delta of each other. */
int gari_vector_equal(const GariVector v1, const GariVector v2, GariFloat delta)
{
  if(!gari_float_equal(v1.x, v2.x, delta)) return FALSE;
  return (gari_float_equal(v1.y, v2.y, delta));
}


/** Vector addition. */
GariVector gari_vector_add(GariVector v1, GariVector v2) {
  return gari_vector(v1.x + v2.x , v1.y + v2.y);
}

/** Vector substraction. */
GariVector gari_vector_sub(GariVector v1, GariVector v2) {
  return gari_vector(v1.x - v2.x , v1.y - v2.y);
}

/** Vector negation. */
GariVector gari_vector_neg(GariVector v1) {
  return gari_vector(-v1.x, -v1.y);
}

/** Scalar Vector multiplication. */
GariVector gari_vector_mul(GariVector v1, GariFloat f) {
  return gari_vector(f * v1.x , f * v1.y);
}

/** Scalar Vector division. */
GariVector gari_vector_div(GariVector v1, GariFloat f) {
  return gari_vector(v1.x / f, v1.y / f);
}

/** Vector dot product. */
GariFloat gari_vector_dot(GariVector v1, GariVector v2) {
  return (v1.x * v2.x + v1.y * v2.y);
}

/** 
Cross product magnitude. The cross product of 2D x,y vectors is 
a 3D vector with a z component, so this function returns only the magnitude of
that z component.  
*/
GariFloat gari_vector_cross(GariVector v1, GariVector v2) {
  return v1.x*v2.y - v1.y*v2.x;
}

/** Perpendicular vector, rotated by 90 degrees, anticlockwise. */
GariVector gari_vector_perp(GariVector v1) {
  return gari_vector(-v1.y, v1.x);
}

/** Perpendicular vector, rotated by -90 degrees, clockwise. */
GariVector gari_vector_nperp(GariVector v1) {
  return gari_vector(v1.y, -v1.x);
}

/** Returns the dot poduct of the vector and itself. */
GariFloat gari_vector_dotself(GariVector v1) {
  return (v1.x * v1.x + v1.y * v1.y); 
}

/** Returns the squared length of the vector. Same as dotself, really. 
Useful for comparing lengths when speed is of importance.  */
GariFloat gari_vector_lensq(GariVector v1) {
  return (v1.x * v1.x + v1.y * v1.y); 
}

/** Returns the length of the vector. */
GariFloat gari_vector_len(GariVector v1) {
  return gari_float_sqrt(v1.x * v1.x + v1.y * v1.y); 
}

/** Returns a vector that is the projection of v1 onto v2*/
GariVector gari_vector_project(GariVector v1, GariVector v2) {
  return gari_vector_mul(v2, gari_vector_dot(v1, v2) / gari_vector_dotself(v2)); 
}

/** Rotates v1 by v2. */
GariVector gari_vector_rotate(GariVector v1, GariVector v2) {
  return gari_vector(v1.x*v2.x-v1.y*v2.y, v1.x*v2.y+v1.y*v2.x);
}  
 
/** Inverse rotation. */
GariVector gari_vector_unrotate(GariVector v1, GariVector v2) {
  return gari_vector(v1.x*v2.x+v1.y*v2.y, v1.x*v2.y-v1.y*v2.x);
}  

/** Linear interpolation on a line between between v1 and v2. Returns a vector 
that points to a point on the line between v1 and v2. */
GariVector gari_vector_lerp(GariVector v1, GariVector v2, GariFloat tx) {
  return gari_vector_add(gari_vector_mul(v1, 1.0f - tx), 
                         gari_vector_mul(v2, tx));
}  

/** Returns v1 normalized. Not safe for division by 0 in case length is 0. */
GariVector gari_vector_normalize_unsafe(GariVector v1) {
  GariFloat len = gari_vector_len(v1);  
  return gari_vector(v1.x / len, v1.y / len);
}


/** Returns v1 normalized. Safe for division by 0 in case length is 0. */
GariVector gari_vector_normalize(GariVector v1) {
  if(v1.x == 0.0f && v1.y == 0.0f) return gari_vector(0.0f, 0.0f);
  return gari_vector_normalize_unsafe(v1);  
}

/** If the length of the vector is greater than max, this function 
returns a vector with the same direction as v1, but with the length 
of that vector limited to max. Otherwise, it returns v1  */
GariVector gari_vector_max(GariVector v1, GariFloat max) {
  GariFloat len = gari_vector_len(v1);
  if (len <= max) return v1;
  return  gari_vector_mul(gari_vector_normalize(v1), max);
}

/** Returns the distance between v1 and v2. */
GariFloat gari_vector_dist(GariVector v1, GariVector v2) {
  return gari_vector_len(gari_vector_sub(v1, v2));
}

/** Returns the squared distance between v1 and v2. */
GariFloat gari_vector_distsq(GariVector v1, GariVector v2) {
  return gari_vector_lensq(gari_vector_sub(v1, v2));
}

/** Returns true if the distance between v1 and v2 is less than d. */
int gari_vector_near(GariVector v1, GariVector v2, GariFloat d) {
  return gari_vector_distsq(v1, v2) < d*d;
}

/** Retuns a unit vector that makes an angle a with the X axis. */
GariVector gari_vector_fromangle(GariFloat f) {
  return gari_vector(gari_float_cos(f), gari_float_sin(f));
} 

/** Returns the angle of the vector with the X axis in clockwise 
direction in radians */
GariFloat gari_vector_angle(GariVector v1) {
  return gari_float_atan2(v1.y, v1.x);
}



