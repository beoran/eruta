#ifndef ALPS_H_INCLUDED
#define ALPS_H_INCLUDED

#include "eruta.h"
#include "camera.h"

struct AlpsDrop_ {
  Point         position;
  Point         velocity;
  double        life;
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
  /*  A reference to the camera is needed because effects are generated 
   * "in view" only. 
   */
  Camera * camera;
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


void alpsshower_init
(AlpsShower * rain, Camera * camera, int intensity, float abberation, Point velocity);
void alpsshower_draw(AlpsShower * rain, Camera * camera);
void alpsshower_update(AlpsShower * rain, double dt);



#endif
