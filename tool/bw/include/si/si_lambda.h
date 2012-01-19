/** SiLambda simulates lambda's for C. */
#ifndef SI_LAMBDA_H
#define SI_LAMBDA_H

struct SiLambda_;
typedef struct SiLambda_ SiLambda;


typedef SiLambda * (SiLambdaFunc)(SiLambda * self);

struct SiLambda_ {
  SiLambdaFunc  * func;
  void          * ptr;
}; 





#endif




