
/** Numercial algorithms. */
#include "si.h"
#include "si_number.h"




uint32_t si_min_uint32(uint32_t a, uint32_t b) {
  return (a < b) ? a : b;
}


uint32_t si_max_uint32(uint32_t a, uint32_t b) {
  return (a > b) ? a : b;
}

uint32_t si_clamp_uint32(uint32_t val, uint32_t min, uint32_t max) {
  if (val < min) return min;
  if (val > max) return max;  
  return val;
}


uint32_t si_min_int32(int32_t a, int32_t b) {
  return (a < b) ? a : b;
}


uint32_t si_max_int32(int32_t a, int32_t b) {
  return (a > b) ? a : b;
}

uint32_t si_clamp_int32(int32_t val, int32_t min, int32_t max) {
  if (val < min) return min;
  if (val > max) return max;  
  return val;
}

double si_min_double(double a, double b) {
  return (a < b) ? a : b;
}


double si_max_double(uint32_t a, double b) {
  return (a > b) ? a : b;
}

double si_clamp_double(double val, double min, double max) {
  if (val < min) return min;
  if (val > max) return max;  
  return val;
}











