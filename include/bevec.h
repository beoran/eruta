#ifndef BEVEC_H_INCLUDED
#define BEVEC_H_INCLUDED

/* Header contains defines for a 2D vector type. */ 

struct BeVec_;
typedef struct BeVec_         BeVec;

/* 2D Vector */
struct BeVec_  {  double x, y;            };

BeVec bevec(double x, double y);
BeVec bevec0();
BeVec bevec_add(BeVec v1, BeVec v2);
BeVec bevec_sub(BeVec v1, BeVec v2);
BeVec bevec_mul(BeVec v1, double factor);
#define bevec_mult bevec_mul

BeVec bevec_neg(BeVec v1);
double  bevec_dot(BeVec v1, BeVec v2);
BeVec bevec_div_unsafe(BeVec v1, double factor);
BeVec bevec_div(BeVec v1, double factor);
double  bevec_lengthsq(BeVec v1);
double  bevec_length(BeVec v1);
BeVec bevec_normalize(BeVec v1);
BeVec bevec_normalize_unsafe(BeVec v1);
BeVec bevec_project(BeVec vec , BeVec on);
BeVec bevec_rightnormal(BeVec vec);
BeVec bevec_leftnormal(BeVec vec);
double  bevec_perproduct(BeVec v1, BeVec v2);
BeVec bevec_forangle(double angle);
double bevec_toangle(BeVec v1);

BeVec bevec_project_2(BeVec self, BeVec other);

BeVec bevec_perp(BeVec self);
BeVec bevec_nperp(BeVec self);
BeVec bevec_project_2(BeVec self, BeVec other);
      
BeVec bevec_rotate(BeVec self, BeVec other);

BeVec bevec_unrotate(BeVec self, BeVec other);
BeVec bevec_lerp(BeVec self, BeVec other, double tx);
BeVec bevec_cliplength(BeVec self, double m);

BeVec bevec_delta(BeVec self, BeVec other);
double bevec_distance(BeVec self, BeVec other); 
double bevec_distsq(BeVec self, BeVec other);
double bevec_near_p(BeVec self, BeVec other, double d);
double bevec_angle(BeVec self);
BeVec bevec_xvec(BeVec self);
BeVec bevec_yvec(BeVec self); 
BeVec bevec_abs(BeVec self);



#endif