
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
  // Check this:
  return bevec(sin(angle), cos(angle));
}

double bevec_toangle(BeVec v1) {
  return atan2(v1.y, v1.x);
}
