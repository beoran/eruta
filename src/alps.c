
#include "alps.h"
#include <stdlib.h>

AlpsDrop * alpsdrop_init(AlpsDrop *drop, Point p, Point v) {
  drop->position = p;
  drop->velocity = v;
  return drop;
}

AlpsDrop * alpsdrop_initrandom(AlpsDrop *drop) {
  Point p = cpv(rand() % SCREEN_W, -(rand() % SCREEN_H));
  Point v = cpv(0.0, 1.0 + ((double)(rand() % 30)) / 10.0);
  return alpsdrop_init(drop, p, v);
}


void alpsdrop_update(AlpsDrop *drop, double dt) {
  drop->position = cpvadd(drop->position, cpvmult(drop->velocity, dt));
  if (drop->position.y > SCREEN_H) {
    alpsdrop_initrandom(drop);
  }
}


void alpsshower_init(AlpsShower * rain, int intensity, float abberation, 
                     Point velocity) {
  int index;
  rain->intensity   = ((intensity > ALPS_SHOWER_DROPS) ?
                       intensity :  ALPS_SHOWER_DROPS);
  rain->abberation  = abberation;
  rain->velocity    = velocity;
  for (index = 0; index < rain->intensity; index ++) {
    alpsdrop_initrandom(rain->drops + index);
  }
}

void alpsdrop_draw(AlpsDrop * drop) {
  // al_put_pixel(drop->position.x, drop->position.y, al_map_rgb(128,128,255));
/*  al_draw_line(drop->position.x, drop->position.y - 5, 
               drop->position.x, drop->position.y,
                al_map_rgb(128,128,255), 2);*/
                
  al_draw_filled_ellipse(drop->position.x, drop->position.y, 
                         1.5, 6.0, al_map_rgb(128,128,255));
}

void alpsshower_draw(AlpsShower * rain) {
  int index;
  for (index = 0 ; index < rain->intensity ; index++) {
    alpsdrop_draw(rain->drops + index);
  }
} 

void alpsshower_update(AlpsShower * rain, double dt) {
  int index;
  for (index = 0; index < rain->intensity; index ++) {
    alpsdrop_update(rain->drops + index, dt);
  }
} 

