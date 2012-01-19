/** Chipmunk helper functions. */
#include <stdlib.h>
#include <string.h>
#include <lauxlib.h>
#include "ch.h"

/** Allocates a rectangular box shape, with the given width and height 
*   and offsets froom center. 
*/

cpPolyShape *
chBoxOffsetInit(cpPolyShape *poly, cpBody *body, 
                cpFloat w, cpFloat h, cpFloat xoff, cpFloat yoff)
{
  cpFloat hw = w/2.0f;
  cpFloat hh = h/2.0f;  
  cpVect  vo = cpv(xoff, yoff);
  
  return chBoxOffsetInit2(poly, body, cpBBNew(-hw, -hh, hw, hh), vo);
}

cpPolyShape *
chBoxOffsetInit2(cpPolyShape *poly, cpBody *body, cpBB box, cpVect offset)
{
  cpVect verts[] = {
    cpv(box.l, box.b),
    cpv(box.l, box.t),
    cpv(box.r, box.t),
    cpv(box.r, box.b),
  };
  
  return cpPolyShapeInit(poly, body, 4, verts, offset);
}

cpShape *
chBoxOffsetNew(cpBody *body, cpFloat width, cpFloat height, 
                cpFloat xoff, cpFloat yoff)
{
  return (cpShape *)chBoxOffsetInit(cpPolyShapeAlloc(), body, width, height, xoff, yoff);
}

cpShape *
chBoxOffsetNew2(cpBody *body, cpBB box, cpVect offset)
{
  return (cpShape *)chBoxOffsetInit2(cpPolyShapeAlloc(), body, box, offset);
}



