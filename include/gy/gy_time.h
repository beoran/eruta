#ifndef GARI_TIME_H
#define GARI_TIME_H


/** Time related functions, including a timer. */
struct GyTimer_;
typedef struct GyTimer_ GyTimer;

struct GyTimer_ {
  uint32_t start;
  uint32_t delay;
};



/** allocates a GyTimer. */
GyTimer * gari_timer_alloc();

/** Frees the timer. */
GyTimer * gari_timer_free(GyTimer * self);

uint32_t gari_timer_ticks();

/** Starts or resets the timer. */
GyTimer * gari_timer_start(GyTimer * self);

/** Sets the delay (wait time) of the timer. */
GyTimer * gari_timer_delay_(GyTimer * self, uint32_t delay);

/** Gets the delay (wait time) of the timer. Returns 0 if self is NULL.*/
uint32_t gari_timer_delay(GyTimer * self);

/** Initializes the timer with a delay in ms. */
GyTimer * gari_timer_init(GyTimer * self, uint32_t delay);

/** Creates a new timer with the given delay. */
GyTimer * gari_timer_new(uint32_t delay);

/** Returns amount of ticks (in ms) passed since the timer was started. */
uint32_t gari_timer_passed(GyTimer * self);


/** Returns true if the delay of the timer has passed since 
gari_timer_start was called, false if not. */
int gari_timer_ring(GyTimer * self);


#endif