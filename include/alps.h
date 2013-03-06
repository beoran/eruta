#ifndef ALPS_H_INCLUDED
#define ALPS_H_INCLUDED

#include "eruta.h"

struct AlpsDrop_ {
  Point position;
  Point velocity;
};


typedef struct AlpsDrop_ AlpsDrop;
typedef struct AlpsShower_ AlpsShower;
typedef struct AlpsPerticle_ AlpsParticle;

#define ALPS_SHOWER_DROPS 2000

struct AlpsShower_ {
  int       intensity;
  float     abberation;
  Point     velocity;
  AlpsDrop  drops[ALPS_SHOWER_DROPS];
};


struct AlpsParticle_ {
  Point             position;
  Point             velocity;
  Point             acceleration;
  int               type;
  double            life;
  double            abberation;
  ALLEGRO_COLOR     color;
  ALLEGRO_BITMAP  * bitmap;
};






#endif
