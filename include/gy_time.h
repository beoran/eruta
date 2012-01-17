#ifndef GY_TIME_H
#define GY_TIME_H


/** Time related functions, including a timer. */
struct GyTimer_;
typedef struct GyTimer_ GyTimer;

struct GyTimer_ {
  uint32_t start;
  uint32_t delay;
};



/** allocates a GyTimer. */
GyTimer * gytimer_alloc();

/** Frees the timer. */
GyTimer * gytimer_free(GyTimer * self);

uint32_t gytimer_ticks();

/** Starts or resets the timer. */
GyTimer * gytimer_start(GyTimer * self);

/** Sets the delay (wait time) of the timer. */
GyTimer * gytimer_delay_(GyTimer * self, uint32_t delay);

/** Gets the delay (wait time) of the timer. Returns 0 if self is NULL.*/
uint32_t gytimer_delay(GyTimer * self);

/** Initializes the timer with a delay in ms. */
GyTimer * gytimer_init(GyTimer * self, uint32_t delay);

/** Creates a new timer with the given delay. */
GyTimer * gytimer_new(uint32_t delay);

/** Returns amount of ticks (in ms) passed since the timer was started. */
uint32_t gytimer_passed(GyTimer * self);


/** Returns true if the delay of the timer has passed since 
gytimer_start was called, false if not. */
int gytimer_ring(GyTimer * self);


#endif