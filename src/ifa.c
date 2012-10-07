
#include "ifa.h"



/** Basic interface for a resorce that can be freed. */ 
struct CanFree_ {
  void * (*free)(void * self);
};


/** Interface that extends CanFree and allows reference counting. */
struct CanReference_ { 
    struct CanFree_ parent;
    void * (*use)(void * self);
    void * (*toss)(void * self);
};



