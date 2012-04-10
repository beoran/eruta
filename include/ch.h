/** Chipmunk helper library */
#ifndef CH_H
#define CH_H

#include "chipmunk.h"




cpPolyShape *
chBoxOffsetInit(cpPolyShape *poly, cpBody *body, 
                cpFloat w, cpFloat h, cpFloat xoff, cpFloat yoff);

cpPolyShape *
chBoxOffsetInit2(cpPolyShape *poly, cpBody *body, cpBB box, cpVect offset);

cpShape *
chBoxOffsetNew(cpBody *body, cpFloat width, cpFloat height, 
                cpFloat xoff, cpFloat yoff);
cpShape *
chBoxOffsetNew2(cpBody *body, cpBB box, cpVect offset);


#endif

