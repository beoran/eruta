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

#endif