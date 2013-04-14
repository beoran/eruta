
#include "bump.h"
#include "eruta.h"
#include <math.h>


BumpVec bumpvec(double x, double y) { 
  BumpVec result = { x, y } ;
  return result;
}

BumpVec bumpvec0() { 
  return bumpvec(0.0, 0.0);
}

BumpVec bumpvec_add(BumpVec v1, BumpVec v2) { 
  return bumpvec(v1.x + v2.x, v1.y + v2.y);
}

BumpVec bumpvec_sub(BumpVec v1, BumpVec v2) { 
  return bumpvec(v1.x - v2.x, v1.y - v2.y);
}

BumpVec bumpvec_mul(BumpVec v1, double factor) { 
  return bumpvec(v1.x * factor, v1.y * factor);
}

BumpVec bumpvec_neg(BumpVec v1) { 
  return bumpvec(-v1.x, -v1.y);
}

double bumpvec_dot(BumpVec v1, BumpVec v2) { 
  return (v1.x * v2.x) + (v1.y * v2.y);
}


BumpVec bumpvec_div_unsafe(BumpVec v1, double factor) { 
  return bumpvec(v1.x / factor, v1.y / factor);
}

BumpVec bumpvec_div(BumpVec v1, double factor) { 
  if (factor == 0.0) {
    return bumpvec0();
  }
  return bumpvec_div_unsafe(v1, factor);
}


double bumpvec_lengthsq(BumpVec v1) { 
  return (v1.x * v1.x) + (v1.y * v1.y); 
}

double bumpvec_length(BumpVec v1) { 
  return sqrt((v1.x * v1.x) + (v1.y * v1.y)); 
}

BumpVec bumpvec_normalize(BumpVec v1) {
  double length = bumpvec_length(v1);
  return bumpvec_div(v1, length);
}

BumpVec bumpvec_normalize_unsafe(BumpVec v1) {
  double length = bumpvec_length(v1);
  return bumpvec_div_unsafe(v1, length);
}

BumpVec bumpvec_project(BumpVec vec , BumpVec on) {
  double dot            = bumpvec_dot(vec, on);
  double lengthsq       = bumpvec_lengthsq(on);
  double x              = on.x * dot / lengthsq;
  double y              = on.y * dot / lengthsq;
  return bumpvec(x, y);
}

BumpVec bumpvec_rightnormal(BumpVec vec) {
  return bumpvec(-vec.y, vec.x);
}

BumpVec bumpvec_leftnormal(BumpVec vec) {
  return bumpvec(vec.y, -vec.x);
}

double bumpvec_perproduct(BumpVec v1, BumpVec v2) {
  return bumpvec_dot(v1, bumpvec_rightnormal(v2));  
}










