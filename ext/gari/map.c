#include "gari.h"

#define GARI_INTERN_ONLY
#include "gari_intern.h"

#ifndef GARI_MAP_MAXLAYER
#define GARI_MAP_MAXLAYER 16
#endif


/**
* A tile map.
*/
struct GariMap_ {
  GariLayer    * layers[GARI_MAP_MAXLAYER];
  GariCamera   * camera;
};





