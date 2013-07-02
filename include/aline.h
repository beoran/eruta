#ifndef ALINE_H_INCLUDED
#define ALINE_H_INCLUDED

typedef struct Aline_ Aline;

struct Aline_ { 
  double c;
  double r;
  double v;
};


Aline aline(double c,double r,double v);

Aline *aline_v(Aline *self,double newv);
Aline *aline_step_i(Aline *self,double dt);
Aline aline_step(Aline self,double dt);
int aline_bump_c(Aline self,Aline other,double *c);
int aline_bump_time(Aline self,Aline other,double *tb);
int aline_negative_bump_time(Aline self,Aline other);
double aline_positive_bump_time(Aline self,Aline other);
int aline_bump_now_p(Aline self,Aline other);
double aline_r2(Aline self);
double aline_hi(Aline self);
double aline_lo(Aline self);


#endif

