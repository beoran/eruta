#ifndef SI_NUMBER_H
#define SI_NUMBER_H
#include "si.h"

/** Clever typed macro that, for A and B of type t, stores the least in res.*/
#define SI_MIN_T(T, A, B, RES)                      \
        {                                           \
          T _a  = (A);  T _b = (B) ;                \
          (RES) = ((_a < _b) ? _a : _b);            \
        } while(0)

/** Clever typed macro that, for A and B of type t, stores the most in res.*/
#define SI_MAX_T(T, A, B, RES)                      \
        {                                           \
          T _a  = (A);  T _b = (B) ;                \
          (RES) = (((_a) > (_b)) ? (_a) : (_b));    \
        } while(0)

/** Clever typed macro that, for V, MIN, MAX, of type T, clamps to res.*/
#define SI_CLAMP_T(T, V, MIN, MAX, RES)                           \
        {                                                         \
          T _min  = (MIN);  T _max = (MAX) ; T _v = V;            \
          (RES) = (_v < _min) ? _min : ((_v > _max) ? _max : _v); \
        } while(0)


#define SI_MIN(A, B) (((A) < (B)) ? (A) : (B))
#define SI_MAX(A, B) (((A) < (B)) ? (A) : (B))
#define SI_CLAMP(V, MIN, MAX) (((V) < (MIN)) ? (MIN) : (SI_MIN(V, MAX)))



/** Returns the least of two unsigned integers. */
uint32_t si_min_uint32(uint32_t a, uint32_t b);
/** Returns the most of two unsigned integers. */
uint32_t si_max_uint32(uint32_t a, uint32_t b);
/** Clamps a value between two unsigned integers. */
uint32_t si_clamp_uint32(uint32_t val, uint32_t min, uint32_t max);

/** Returns the least of two signed integers. */
uint32_t si_min_int32(int32_t a, int32_t b);
/** Returns the most of two signed integers. */
uint32_t si_max_int32(int32_t a, int32_t b);
/** Clamps a value between two signed integers. */
uint32_t si_clamp_int32(int32_t val, int32_t min, int32_t max);

/** Returns the least of two doubles. */
double si_min_double(double a, double b);
/** Returns the most of two doubles. */
double si_max_double(uint32_t a, double b);
/** Clamps a value between two doubles. */
double si_clamp_double(double val, double min, double max);


#endif

