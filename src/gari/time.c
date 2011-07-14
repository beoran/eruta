#include <stdlib.h>

#include "gari.h"
#define GARI_INTERN_ONLY
#include "gari_intern.h"
#include <time.h>


/** Time related functions, including a timer. */


struct GariTimer_;
typedef struct GariTimer_ GariTimer;


struct GariTimer_ {
  uint32_t start;
  uint32_t delay;
};

GariTimer * gari_timer_alloc() {
  return (GariTimer *)GARI_MALLOC(sizeof(GariTimer));
}

/** Frees the timer. */
GariTimer * gari_timer_free(GariTimer * self) {
  GARI_FREE(self);
  return NULL;
}

uint32_t gari_timer_ticks() { 
  return (uint32_t) SDL_GetTicks();
}  

/** Starts or resets the timer. */
GariTimer * gari_timer_start(GariTimer * self) {
  if(!self) return NULL;
  self->start = gari_timer_ticks();
  return self;
}

/** Sets the delay (wait time) of the timer. */
GariTimer * gari_timer_delay_(GariTimer * self, uint32_t delay) {
  if (!self) return NULL;
  self->delay = delay;
  return self; 
} 

/** Gets the delay (wait time) of the timer. Returns 0 if self is NULL.*/
uint32_t gari_timer_delay(GariTimer * self) {
  if(!self) return 0;
  return self->delay;
}

/** Initializes the timer with a delay in ms. */
GariTimer * gari_timer_init(GariTimer * self, uint32_t delay) {
  if(!self) return NULL;
  self->start = 0;
  self->delay = delay;
  return gari_timer_start(self);
}

/** Creates a new timer with the given delay. */
GariTimer * gari_timer_new(uint32_t delay) {
  return gari_timer_init(gari_timer_alloc(), delay);
}

/** Returns amount of ticks (in ms) passed since the timer was started. */
uint32_t gari_timer_passed(GariTimer * self) {
  uint32_t now = gari_timer_ticks();
  return (now - self->start);
} 

/** Returns true if the delay of the timer has passed since 
gari_timer_start was called, false if not. */
int gari_timer_ring(GariTimer * self) {
  return (gari_timer_passed(self) >= self->delay);
} 









