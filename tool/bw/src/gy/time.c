#include <stdlib.h>

#include "gy.h"
#define GY_INTERN_ONLY
#include "gy_intern.h"
#include <time.h>


/** Time related functions, including a timer. */


/*
struct GyTimer_;
typedef struct GyTimer_ GyTimer;
*/

struct GyTimer_ {
  uint32_t start;
  uint32_t delay;
};


GyTimer * gytimer_alloc() {
  return (GyTimer *)GY_MALLOC(sizeof(GyTimer));
}

/** Frees the timer. */
GyTimer * gytimer_free(GyTimer * self) {
  GY_FREE(self);
  return NULL;
}

uint32_t gytimer_ticks() { 
  return (uint32_t) SDL_GetTicks();
}  

/** Starts or resets the timer. */
GyTimer * gytimer_start(GyTimer * self) {
  if(!self) return NULL;
  self->start = gytimer_ticks();
  return self;
}

/** Sets the delay (wait time) of the timer. */
GyTimer * gytimer_delay_(GyTimer * self, uint32_t delay) {
  if (!self) return NULL;
  self->delay = delay;
  return self; 
} 

/** Gets the delay (wait time) of the timer. Returns 0 if self is NULL.*/
uint32_t gytimer_delay(GyTimer * self) {
  if(!self) return 0;
  return self->delay;
}

/** Initializes the timer with a delay in ms. */
GyTimer * gytimer_init(GyTimer * self, uint32_t delay) {
  if(!self) return NULL;
  self->start = 0;
  self->delay = delay;
  return gytimer_start(self);
}

/** Creates a new timer with the given delay. */
GyTimer * gytimer_new(uint32_t delay) {
  return gytimer_init(gytimer_alloc(), delay);
}

/** Returns amount of ticks (in ms) passed since the timer was started. */
uint32_t gytimer_passed(GyTimer * self) {
  uint32_t now = gytimer_ticks();
  return (now - self->start);
} 

/** Returns true if the delay of the timer has passed since 
gytimer_start was called, false if not. */
int gytimer_ring(GyTimer * self) {
  return (gytimer_passed(self) >= self->delay);
} 









