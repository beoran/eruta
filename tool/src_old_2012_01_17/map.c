#include "gy.h"

#define GY_INTERN_ONLY
#include "gy_intern.h"

#ifndef GY_MAP_MAXLAYER
#define GY_MAP_MAXLAYER 16
#endif


/**
* A tile map.
*/
struct GyMap_ {
  GyLayer    * layers[GY_MAP_MAXLAYER];
  GyCamera   * camera;
};





