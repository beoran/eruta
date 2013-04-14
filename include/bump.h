#ifndef bump_H_INCLUDED
#define bump_H_INCLUDED
#include "../src/xmlragel.rl"


struct BumpVec;
typedef struct BumpVec_         BumpVec;
struct BumpAABB;
typedef struct BumpVec_         BumpAABB;
typedef struct BumpBody_        BumpBody;
typedef struct BumpHull_        BumpHull;
typedef struct BumpHoopHull_    BumpHull;
typedef struct BumpBoxHull_     BumpHull;


struct BumpVec_  {  double x, y;            };
struct BumpAABB_ {  BumpVec p; BumpVec hs;  };

struct BumpBody_ {
  BumpVec p;
  BumpVec v;
  BumpVec a;
  float   m;
  BumpVec p_next;
  BumpVec v_next;
  BumpVec a_next;
};

struct BumpHull_ {   
  BumpBody_ body;
  BumpVec   delta;
};


BumpVec bumpvec(double x, double y);
BumpVec bumpvec0();
BumpVec bumpvec_add(BumpVec v1, BumpVec v2);
BumpVec bumpvec_sub(BumpVec v1, BumpVec v2);
BumpVec bumpvec_mul(BumpVec v1, double factor);
BumpVec bumpvec_neg(BumpVec v1);
double  bumpvec_dot(BumpVec v1, BumpVec v2);
BumpVec bumpvec_div_unsafe(BumpVec v1, double factor);
BumpVec bumpvec_div(BumpVec v1, double factor);
double  bumpvec_lengthsq(BumpVec v1);
double  bumpvec_length(BumpVec v1);
BumpVec bumpvec_normalize(BumpVec v1);
BumpVec bumpvec_normalize_unsafe(BumpVec v1);
BumpVec bumpvec_project(BumpVec vec , BumpVec on);
BumpVec bumpvec_rightnormal(BumpVec vec);
BumpVec bumpvec_leftnormal(BumpVec vec);
double  bumpvec_perproduct(BumpVec v1, BumpVec v2);




#endif




