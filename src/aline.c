#include "aline.h"

/* An Aline is an axis-oriended line segment that has it's center in c
 * and a "radius" r, andt hat mis mooving at a speed v. It's used to split up 
 * collision problems into simpler  1-dimensional solutions.
 */

/* Equal to c - r */
double aline_lo(Aline self) { 
  return  self.c - self.r;
}
    
/* Equal to c + r */
double aline_hi(Aline self) { 
  return self.c + self.r;
} 


    
/* Equal to r + r */
double aline_r2(Aline self) { 
  return self.r + self.r;
}

/* Unicode warning: I used unicode characters in the comments for this class
   for deriving the maths. The characters on the next line should all be 
   readable: Δ⁰¹²³⁴⁵⁶⁷⁸⁹⁺⁻⁼⁽⁾ ₀₁₂₃₄₅₆₇₈₉ ₊₋₌₍₎ᵃᵇᶜᵈᵉᶠᵍʰⁱʲᵏˡᵐⁿᵒᵖʳˢᵗᵘᵛʷˣʸᶻₐₑₒₓᵅᵋᶿᶥᶲ.
*/    
    
    /* Initialize a new Aline with position c, radius r and velocity v */
Aline aline(double c, double r, double v) {
  Aline self;
  self.r = r;
  self.c = c;
  self.v = v;
  return self;
}

/* Returns true if self is overlapping with other right now, 
   at the current position, false if not 
*/
int aline_bump_now_p(Aline self, Aline other) {
  if (self.c == other.c) return 1;
  if (self.c < other.c)  /* positive side collision */
        return (self.c + self.r) >= (other.c - other.r);
  else  /* self.c > other.c, so negative side collision */
        return (self.c - self.r) <= (other.c + other.r);
}
    
/* Calculates bump time for collisions on the positive side of self. 
 * Returns 0 if no bump time could be calculated.
 */
double aline_positive_bump_time(Aline self, Aline other) { 
      /* Δtᵇ is the time of collision calculated from now, as we define t⁰ == 0.
      # self.cᵇ is the position of the center of self at collision time Δtᵇ.
      # other.cᵇ is the position of the center of other at collision time Δtᵇ.
      #  
      # Now at collision on the positive side:
      #  
      # self.cᵇ + self.r   == other.cᵇ - other.r 
      # self.cᵇ - other.cᵇ == - self.r - other.r (1)
      # 
      # Now, self.v is kept constant during Δt, so:
      # 
      # self.v            == Δself.c / Δt 
      # self.v            == (self.cᵇ - self.c⁰) / (tᵇ - t⁰ self.cᵇ) 
      #   t₀ == 0, so:  
      # self.v            == (self.cᵇ - self.c⁰) / tᵇ
      # self.cᵇ - self.c⁰ == self.v * tᵇ
      # self.cᵇ           == self.v * tᵇ + self.c⁰ (2)
      #
      #
      # the same goes for other so: 
      # other.v           == (other.cᵇ - other.c⁰) / tᵇ
      # other.cᵇ          == other.v * tᵇ + other.c⁰ (3)
      #
      # Substitute (2) and (3) into (1): 
      # self.v * tᵇ + self.c⁰ - other.v * tᵇ - other.c⁰ == - self.r - other.r
      # self.v * tᵇ - other.v * tᵇ + self.c⁰ - other.c⁰ == - self.r - other.r
      # (self.v - other.v) * tᵇ  + (self.c⁰ - other.c⁰) == - self.r - other.r
      # (self.v - other.v) * tᵇ  == -self.r - other.r - self.c⁰ + other.c⁰
      # (reversing the sign): 
      # (other.v - self.v) * tᵇ  ==  self.r + other.r + (self.c⁰ - other.c⁰)
      # let vrel = (other.v - self.v)  
      # and crel = (other.c⁰ - self.c⁰)
      # and rsum = self.r + other.r
      # then:
      # vrel * tᵇ  ==  rsum - crel
      # if  vrel != 0 
      # tᵇ  ==  (rsum - crel) / vrel
      */
      double vrel = self.v - other.v; /* Relative speed. */
      double crel = self.c - other.c; /* Relative position. */
      double rsum = self.r + other.r; /* Sum of the radiuses. */
      double tb   = (rsum - crel) / vrel;
      return tb;      
}


/* Calculates bump time for collisions on the negative side of self */
int aline_negative_bump_time(Aline self, Aline other) {
      /* Δtᵇ is the time of collision calculated from now, as we define t⁰ == 0.
      # self.cᵇ is the position of the center of self at collision time Δtᵇ.
      # other.cᵇ is the position of the center of other at collision time Δtᵇ.
      #  
      # Now at collision from the negative side:
      #  
      # self.cᵇ - self.r   == other.cᵇ + other.r 
      # self.cᵇ - other.cᵇ == self.r + other.r (1)
      # 
      # Now, self.v is kept constant during Δt, so:
      # 
      # self.v            == Δself.c / Δt 
      # self.v            == (self.cᵇ - self.c⁰) / (tᵇ - t⁰ self.cᵇ) 
      #   t₀ == 0, so:  
      # self.v            == (self.cᵇ - self.c⁰) / tᵇ
      # self.cᵇ - self.c⁰ == self.v * tᵇ
      # self.cᵇ           == self.v * tᵇ + self.c⁰ (2)
      #
      #
      # the same goes for other so: 
      # other.v           == (other.cᵇ - other.c⁰) / tᵇ
      # other.cᵇ          == other.v * tᵇ + other.c⁰ (3)
      #
      # Substitute (2) and (3) into (1): 
      # self.v * tᵇ + self.c⁰ - other.v * tᵇ - other.c⁰ == self.r + other.r
      # self.v * tᵇ - other.v * tᵇ + self.c⁰ - other.c⁰ == self.r + other.r
      # (self.v - other.v) * tᵇ  + (self.c⁰ - other.c⁰) == self.r + other.r
      # (self.v - other.v) * tᵇ  ==  self.r + other.r - self.c⁰ + other.c⁰
      # (reversing the sign): 
      # (other.v - self.v) * tᵇ  ==  - self.r - other.r + (self.c⁰ - other.c⁰)
      # let vrel = (other.v - self.v)  
      # and crel = (other.c⁰ - self.c⁰)
      # and rsum = self.r + other.r
      # then:
      # vrel * tᵇ  ==  - rsum - crel
      # if  vrel != 0 
      # tᵇ  ==  (- rsum - crel) / vrel
      # tᵇ  ==  -( rsum + crel) / vrel
      */
      double vrel = self.v - other.v; /* Relative speed. */
      double crel = self.c - other.c; /* Relative position. */
      double rsum = self.r + other.r; /* Sum of the radiuses. */
      double tb   = (-(rsum + crel)) / vrel;
      return tb;
}
    
    
/* Calculates the time when self will bump into other. Returns nonzero
  *and stores the time in tb if  there is a collision. Returns zero if there is 
  no collision possible, for example, in the case their speeds are the same,
  in which case they will either  never collide,  or always were and always 
  will be colliding, assuming their speed does not change.     
  If the bump time is negative, the initial collision took place in the 
  past, and may still be ongoing. 
*/
int aline_bump_time(Aline self, Aline other, double * tb) { 
  /* Two objects moving at equal speed who are not already colliding
    will never collide with each other. */
  double res;
  if (self.v == other.v) return 0;
  
  if (self.c <= other.c) 
  /* The collision will happen or has happened on the positive 
      side of self at self.c + self.r and at the negative side of 
      other at other.c - other.r
    */
    res =  aline_negative_bump_time(self, other);
  else /* here self.c > other.c */
    /* The collision will happen or has happened on the negative 
      side of self at self.c - self.r and at the positive side of 
      other at other.c + other.r */
    res = aline_positive_bump_time(self, other);
  if (tb) (*tb) = res;
  return !0;
}


/* Caluclates the prediced position of c where self will bump into other
  if both retain the same speeds. Returns 0 if their speeds are the same 
  in which case they will either never collide, or always stay in overlap.  
  */
int aline_bump_c(Aline self, Aline other, double * c) {
  double tb, cb;
  if(!aline_bump_time(self, other, &tb)) return 0;
  /* self.cᵇ          == self.v * tᵇ + self.c⁰ (3) */
  cb     = self.v * tb + self.c;
  if(c) (*c) = cb;
  return !0;
}

/* Simulates a motion that takes place during a time interval dt (Δt)
* assuming the speed remained constant during that interval.
* Returns a new line piece aline with it's c value updated to reflect 
* the changed position.
* @return [Aline] 
*/
Aline aline_step(Aline self, double dt) { 
  double dc    = dt * self.v;
  double newc  = self.c + dc;
  return aline(newc, self.r, self.v);
}

/* Simulates a motion as per aline_step, but modifies self. 
* @return [Aline]
*/
Aline * aline_step_i(Aline * self, double dt) {
  double dc    = dt * self->v;
  self->c      = self->c + dc;
  return self;
}


/* Sets the speed of the Aline. */
Aline * aline_v(Aline * self, double newv) { 
  self->v = newv;
  return self;
}







