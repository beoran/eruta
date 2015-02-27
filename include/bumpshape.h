#ifndef bumpshape_H_INCLUDED
#define bumpshape_H_INCLUDED

/** Shapes and bounds boxes for the bump physics system.
 * These are all small structs intended to be used as value types.
 */

#include <stdarg.h>
#include "bevec.h"


struct BumpAABB;
typedef struct BumpAABB_        BumpAABB;
typedef struct BumpCircle_      BumpCircle;
typedef struct BumpSegment_     BumpSegment;

typedef struct BumpBoxShape_      BumpBoxShape;
typedef struct BumpCircleShape_   BumpCircleShape;
typedef struct BumpSegmentShape_  BumpSegmentShape;
typedef struct BumpErrorShape_    BumpErrorShape;
typedef union  BumpShape_         BumpShape;

/* Axis Aligned bounds box. */
struct BumpAABB_   {  BeVec p; BeVec hs;  };

/* Circle. */
struct BumpCircle_ {  BeVec p; double radius; };

/* Line segment. Segment stretches from p1 to p2  */
struct BumpSegment_ {  BeVec p1; BeVec p2; };

/* Types of BumpShapes */
enum BumpShapeType_ {
  BUMPSHAPE_BOX      = 0,
  BUMPSHAPE_CIRCLE   = 1,
  BUMPSHAPE_SEGMENT  = 2,
  BUMPSHAPE_ERROR    = 69,
};

/* Shapes */
struct BumpErrorShape_ {
  int         type;
  int         code;
};


/* Shapes */
struct BumpBoxShape_ {
  int         type;
  BumpAABB    shape;
};

/* Shapes */
struct BumpCircleShape_ {
  int         type;
  BumpCircle  shape;
};

/* Shapes */
struct BumpSegmentShape_ {
  int         type;
  BumpSegment shape;
};

/* Shape type. Uses the overlapping structs in a union trick. */
union BumpShape_ {
  BumpBoxShape      box;
  BumpCircleShape   circle;
  BumpSegmentShape  segment;
  BumpErrorShape    error;
};

BumpAABB bumpaabb(double cx, double cy, double w, double h);
BumpAABB bumpaabb_make_int(int x, int y, int w, int h);

BumpCircle bumpcircle(double cx, double cy, double radius);
BumpCircle bumpcircle_make_int(int x, int y, int radius);

BumpSegment bumpsegment(double x1, double y1, double x2, double y2);
BumpSegment bumpsegment_make_int(int x1, int y1, int x2, int y2);

BumpShape bumpcircleshape(double cx, double cy, double radius);
BumpShape bumpsegmentshape(double x1, double y1, double x2, double y2);
BumpShape bumpboxshape(double cx, double cy, double w, double h);

BumpShape bumpshape_va(int type, va_list args);
BumpShape bumpshape(int type, ...);

double bumpaabb_left(BumpAABB * self);
double bumpaabb_right(BumpAABB * self);
double bumpaabb_top(BumpAABB * self);
double bumpaabb_down(BumpAABB * self);
double bumpaabb_width(BumpAABB * self);
double bumpaabb_height(BumpAABB * self);


double bumpaabb_overlap_x(BumpAABB self, BumpAABB other);

/* Returns overlap of the bounds boxes of self and other in y direction. 
 * May be zero or negative if no overlap.
 */ 
double bumpaabb_overlap_y(BumpAABB self, BumpAABB other);

/* Returns overlap of the bounds boxes of self and other in x direction. 
 * May be zero or negative if no overlap.
 */ 
double bumpaabb_overlap_x_raw(BumpAABB self, BumpAABB other);

/* Returns overlap of the bounds boxes of self and other in y direction. 
 * May be zero or negative if no overlap.
 */ 
double bumpaabb_overlap_y_raw(BumpAABB self, BumpAABB other);

/* Returns the overlap vector between two bounds boxes. */
BeVec bumpaabb_overlap_vector(BumpAABB self, BumpAABB other);

/* Returns whether or not there is overlap between two bounds boxes. */
int bumpaabb_overlap_p(BumpAABB self, BumpAABB other);



#endif




