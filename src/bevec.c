
/* 2D Vector types implementation. */

#include <math.h>
#include "bevec.h"


BeVec bevec(double x, double y) { 
  BeVec result = { x, y } ;
  return result;
}

BeVec bevec0() { 
  return bevec(0.0, 0.0);
}

BeVec bevec_add(BeVec v1, BeVec v2) { 
  return bevec(v1.x + v2.x, v1.y + v2.y);
}

BeVec bevec_sub(BeVec v1, BeVec v2) { 
  return bevec(v1.x - v2.x, v1.y - v2.y);
}

BeVec bevec_mul(BeVec v1, double factor) { 
  return bevec(v1.x * factor, v1.y * factor);
}

BeVec bevec_neg(BeVec v1) { 
  return bevec(-v1.x, -v1.y);
}

/* Dot product. */
double bevec_dot(BeVec v1, BeVec v2) { 
  return (v1.x * v2.x) + (v1.y * v2.y);
}


BeVec bevec_div_unsafe(BeVec v1, double factor) { 
  return bevec(v1.x / factor, v1.y / factor);
}

BeVec bevec_div(BeVec v1, double factor) { 
  if (factor == 0.0) {
    return bevec0();
  }
  return bevec_div_unsafe(v1, factor);
}


double bevec_lengthsq(BeVec v1) { 
  return (v1.x * v1.x) + (v1.y * v1.y); 
}

double bevec_length(BeVec v1) { 
  return sqrt((v1.x * v1.x) + (v1.y * v1.y)); 
}

BeVec bevec_normalize(BeVec v1) {
  double length = bevec_length(v1);
  return bevec_div(v1, length);
}

BeVec bevec_normalize_unsafe(BeVec v1) {
  double length = bevec_length(v1);
  return bevec_div_unsafe(v1, length);
}

BeVec bevec_project(BeVec vec , BeVec on) {
  double dot            = bevec_dot(vec, on);
  double lengthsq       = bevec_lengthsq(on);
  double x              = on.x * dot / lengthsq;
  double y              = on.y * dot / lengthsq;
  return bevec(x, y);
}

BeVec bevec_rightnormal(BeVec vec) {
  return bevec(-vec.y, vec.x);
}

BeVec bevec_leftnormal(BeVec vec) {
  return bevec(vec.y, -vec.x);
}

double bevec_perproduct(BeVec v1, BeVec v2) {
  return bevec_dot(v1, bevec_rightnormal(v2));  
}

BeVec bevec_forangle(double angle) {
  return bevec(cos(angle), sin(angle));
}

/* Returns a unit vector that makes the given angle a with the X axis. */
BeVec bevec_for_angle(double a) {       
  return bevec(cos(a), sin(a));
}

double bevec_toangle(BeVec v1) {
  return atan2(v1.y, v1.x);
}

/* Cross product magnitude. The cross product of 2D x,y vectors is
   a 3D vector with a pure z component, so this function returns only the     
  magnitude of that z component.  */
double bevec_cross(BeVec self, BeVec other) { 
  return self.x * other.y - self.y * other.x; 
}


/* Perpendicular vector, rotated by 90 degrees, anticlockwise. */
BeVec bevec_perp(BeVec self) { 
  return bevec(-self.y, self.x);
}

/* Perpendicular vector, rotated by -90 degrees, clockwise. */
BeVec bevec_nperp(BeVec self) { 
  return bevec(self.y, -self.x);
}
 
 /* Returns a vector that is the projection of self onto other. 
  * Not possible if other has a length 0. */
BeVec bevec_project_2(BeVec self, BeVec other) {
  double dot_product, other_length;
  dot_product   = bevec_dot(self, other);
  other_length  = bevec_length(other);
  return bevec_mul(other, dot_product  / other_length);
}

      
/* Rotates self by other. */
BeVec bevec_rotate(BeVec self, BeVec other) {
  return bevec(self.x*other.x - self.y*other.y, 
               self.x*other.y + self.y*other.x);
}
 
/* Inverse rotation. */
BeVec bevec_unrotate(BeVec self, BeVec other) {
  return bevec(self.x*other.x + self.y*other.y, 
               self.x*other.y - self.y*other.x);
}
 
/* Linear interpolation on a line between between self and other. 
 * Returns a vector that points to a point on the line between self and other. */
BeVec bevec_lerp(BeVec self, BeVec other, double tx) { 
  return bevec_add(bevec_mul(self, 1.0 - tx), bevec_mul(other, tx)); 
  /* self * (1.0 - tx) + (other * tx) */
}
    
/* If the length of the vector is greater than max, this function 
    returns a vector with the same direction as self, but with the length 
    of that vector limited to max. Otherwise, it returns a copy of self.  
*/
BeVec bevec_cliplength(BeVec self, double m) { 
  double len  = bevec_length(self);
  if (len <= m) return self;
  return bevec_mul(bevec_normalize(self), m);
}

/* Returns the vector between the two points represented by the vectors self and other. */
BeVec bevec_delta(BeVec self, BeVec other) {
  return bevec_sub(self, other);
} 

/* Returns the distance between the two points represented by the vectors self and other. */ 
double bevec_distance(BeVec self, BeVec other) {
  return bevec_length(bevec_delta(self, other));
} 

/* Returns the distance squared between the two points represented by the vectors self and other. */ 
double bevec_distsq(BeVec self, BeVec other) {
  return bevec_lengthsq(bevec_delta(self, other));
} 

/* Returns true if the distance between self and other is less than or 
  equal to d.
*/
double bevec_near_p(BeVec self, BeVec other, double d) {
  return bevec_distsq(self, other) <= (d*d);
}
  

/* Returns the angle of the vector with the X axis in clockwise 
 * direction in radians
 */
double bevec_angle(BeVec self) { 
  return atan2(self.y, self.x);
}
 
 /* Returns a vector with components self.x, 0 */
BeVec bevec_xvec(BeVec self) { 
      return bevec(self.x, 0.0);
}

/* Returns a vector with components 0, self.y */
BeVec bevec_yvec(BeVec self) { 
      return bevec(0.0, self.y);
}
 
/*    
  Returns the overlap of the x component of self, of other, where  
  self and other are centers of line pieces with width rself and rother 
  respectivly. Returned as a vector with only an x component with the 
  magnitude of overlap.
BeVec overlap_x(BeVec self, BeVec other,rself, rother) 
      xself   = self.vector_x
      xother  = other.vector_x
      return zero unless xself.near(xother, rself)
      return zero unless xother.near(xself, rother)
    end
*/



//  public domain function by Darel Rex Finley, 2006

//  Determines the intersection point of the line segment defined by points A and B
//  with the line segment defined by points C and D.
//
//  Returns YES if the intersection point was found, and stores that point in X,Y.
//  Returns NO if there is no determinable intersection point, in which case X,Y will
//  be unmodified.
/*
int bevec_intersect_segment(BeVec A, BeVec B, BeVec C, BeVec D, BeVeC * result) { 

  double  dist_AB, aid_cos, aid_sin, newX, ABpos ;

  //  Fail if either line segment is zero-length.
  if (A.x == B.x && A.y == B.y || C.x == D.x && C.y == D.y) return NO;

  //  Fail if the segments share an end-point.
  if (A.x==C.x && A.y==C.y || B.x==C.x && B.y==C.y
  ||  A.x==D.x && A.y==D.y || B.x==D.x && B.y==D.y) {
    return NO; }

  //  (1) Translate the system so that point A is on the origin.
  B = bevec_sub(B, A);
  C = bevec_sub(C, A);
  D = bevec_sub(D, A);
  
  
  //  Discover the length of segment A-B.
  dist_AB  = bevec_length(B);
  
  //  (2) Rotate the system so that point B is on the positive X axis.
  aid_Cos=Bx/distAB;
  aid_sin =By/distAB;
  newX=Cx*theCos+Cy* aid_sin;
  Cy  =Cy*theCos-Cx* aid_sin; Cx=newX;
  newX=Dx*theCos+Dy* aid_sin;
  Dy  =Dy*theCos-Dx* aid_sin; Dx=newX;

  //  Fail if segment C-D doesn't cross line A-B.
  if (Cy<0. && Dy<0. || Cy>=0. && Dy>=0.) return NO;

  //  (3) Discover the position of the intersection point along line A-B.
  ABpos=Dx+(Cx-Dx)*Dy/(Dy-Cy);

  //  Fail if segment C-D crosses line A-B outside of segment A-B.
  if (ABpos<0. || ABpos>distAB) return NO;

  //  (4) Apply the discovered position to line A-B in the original coordinate system.
  *X=Ax+ABpos*theCos;
  *Y=Ay+ABpos* aid_sin;

  //  Success.
  return YES; }
  */