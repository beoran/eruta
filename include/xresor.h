#ifndef xresor_H_INCLUDED
#define xresor_H_INCLUDED

#include "resor.h"

void * xresor_tilemap_loader(const char * vpath, void * extra_);
int xresor_tilemap_destructor(Resor * resor);
Resor * xresor_load_tilemap(const char * vpath, void * extra);




#endif




