
#include "bevec.h"
#include "eruta.h"
#include "dynar.h"
#include "rebox.h"
#include "sprite.h"
#include "area.h"
#include "thing.h"
#include "camera.h"
#include "alps.h"

#include <stdlib.h>


/* Planned effects:  
 * 1) Damage display. (Can be linked to the SpriteState also.)
 * 2) Weather: Rain, snow, hail. (each of these is exclusive) 
 * 3) Sunlight: "American nights" (simple color filter over all) , 
 *    komorebi (forest light), brightness (white over all).
 * 4) Slashes. (Could be linked to the SpriteState also). 
 * 5) Spell effects: Star bursts 
 * 
 * 6) Aura colors to indicate forcefields, status effects, etc. 
 * Definitely to be linked tho the spritestate. Probably doesn't belong in Alps.  
 *
 * 
 */



AlpsDrop * alpsdrop_init(AlpsDrop *drop, Point p, Point v, double life) {
  drop->position = p;
  drop->velocity = v;
  drop->life     = life;
  return drop;
}


int rand_betweeni(int low, int high) {
  return low + (rand() % (high - low));
}

double rand_betweend(double low, double high) {
  return low + (high - low) * ((double)rand() / (double) RAND_MAX);
}

Point rand_point(Point low, Point high) {
  double x, y;
  x = rand_betweend(low.x, high.x);
  y = rand_betweend(low.y, high.y);
  return bevec(x, y);  
}



AlpsDrop * alpsdrop_initrandom(AlpsDrop *drop, Camera * camera) {
  Point p = rand_point(camera_at(camera), camera_br(camera));
  Point v = bevec(0.0, 80.0 + ((double)(rand() % 600)) / 10.0);
  Point pw = camera_screentoworld(camera, p);
  double life = 1.0 + ((double)(rand() % 50)) / 10.0;
  return alpsdrop_init(drop, pw, v, life);
}


/* Update an alpsdrop. This also takes the calera as a parameter, since 
 for efficiency, and since they are not critical effects are generated 
 only inside the camera view, at positions related to it. O,nvce they leave the 
 camera view, the effects are disabled. */
void alpsdrop_update(AlpsDrop *drop, Camera * camera,  double dt) {
  Point screenp;
  drop->position = bevec_add(drop->position, bevec_mul(drop->velocity, dt));
  screenp = camera_worldtoscreen(camera, drop->position);
  drop->life -= dt;
  if (drop->life <= 0.0) { 
    alpsdrop_initrandom(drop, camera);
  }
}


void alpsshower_init(AlpsShower * rain, Camera * camera, int intensity, float abberation, 
                     Point velocity) {
  int index;
  rain->camera      = camera;
  rain->intensity   = ((intensity < ALPS_SHOWER_DROPS) ?
                       intensity :  ALPS_SHOWER_DROPS);
  rain->abberation  = abberation;
  rain->velocity    = velocity;
  for (index = 0; index < rain->intensity; index ++) {
    alpsdrop_initrandom(rain->drops + index, camera);
  }
}

void alpsdrop_draw(AlpsDrop * drop, Camera * camera) {
  // al_put_pixel(drop->position.x, drop->position.y, al_map_rgb(128,128,255));
/*  al_draw_line(drop->position.x, drop->position.y - 5, 
               drop->position.x, drop->position.y,
                al_map_rgb(128,128,255), 2);*/
  Point drawp = camera_worldtoscreen(camera, drop->position);  

  al_draw_filled_ellipse(drawp.x, drawp.y, 
                         1.5, 6.0, al_map_rgba(128,128,255,191));
}

/* XXX: camera is redundant. */
void alpsshower_draw(AlpsShower * rain, Camera * camera) {
  int index;
  for (index = 0 ; index < rain->intensity ; index++) {
    alpsdrop_draw(rain->drops + index, rain->camera);
  }
} 

void alpsshower_update(AlpsShower * rain, double dt) {
  int index;
  for (index = 0; index < rain->intensity; index ++) {
    alpsdrop_update(rain->drops + index, rain->camera, dt);
  }
} 




