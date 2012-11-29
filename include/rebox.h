#ifndef rebox_H_INCLUDED
#define rebox_H_INCLUDED


#include "eruta.h"

typedef struct Rebox_ Rebox;

/** Rebox is short for REctangular BOX. It's a rectangular 
box with given position and size. */
struct Rebox_ {
  Point at;
  Point size;
};


#include "rebox_proto.h"



#endif




