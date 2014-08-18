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
typedef struct AlpsParticle_ AlpsParticle;
typedef struct Alps_ Alps;

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

/* Allegro Lighting and Particle system. Cosmetic effects.
 * Detail of planned effects per category:
 * 1) Precipitation (shower) :
 *    - Rain
 *    - Snow
 *    - Hail
 *    - Flower Petals
 *    - Leaves
 *    => Draw a line/circle for rain, snow, hail, or use a single
 *       repeated bitmap for leaves or petals, or arbitrary rain-like effects.
 *    => Motion: rain + hail fast, others slower, speed, wind effect and
 *       "billowing" are needed.
 * 2) Sun Light Effects
 *    - Sun beams
 *    - Komorebi
 *    => statical effects
 * 3) Night effects / filters
 *    - "American night" filter / color filter
 *    => likewise static effect.
 * => 1 to 3: whole screen / camera view effect
 *
 * 4) True particle effects (only now we get there)
 *    - Explosions
 *    - Foaming water
 * => effect near an emmitter / SpriteState
 **/
struct Alps_ {
  struct AlpsShower_ shower;
};

void alps_init(Alps * self, Camera * camera);
void alps_done(Alps * self);
void alps_update(Alps * self, double dt);
void alps_draw(Alps * self);
void alps_start_shower(Alps * self);
void alps_stop_shower(Alps * self);



#endif
