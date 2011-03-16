module Bump
  # An aline is an axis oriented line piece centered around a point c
  # with a radius r, that has an initial speed v.
  # If the line is vertical  , c corresponds to y and r to h/2.
  # if the line is horizontal, c corresponds to x, and r to w/2.   
  class Aline
    attr_reader :c
    attr_reader :r
    attr_reader :v
    
    # Unicode warning: I used unicode characters in the comments for this class
    # for deriving the maths. The characters on the next line should all be 
    # readable: Δ⁰¹²³⁴⁵⁶⁷⁸⁹⁺⁻⁼⁽⁾ ₀₁₂₃₄₅₆₇₈₉ ₊₋₌₍₎ᵃᵇᶜᵈᵉᶠᵍʰⁱʲᵏˡᵐⁿᵒᵖʳˢᵗᵘᵛʷˣʸᶻₐₑₒₓᵅᵋᶿᶥᶲ.
    
    
    # Initialize a new Aline with position c, radius r and velocity v
    def initialize(c, r, v) 
      @c = c.to_f
      @r = r.to_f
      @v = v.to_f
    end
    
    # Returns true if self is overlapping with other right now, 
    # at the current position, fale if not 
    def bump_now?(other)
      return true if self.c == other.c
      if self.c < other.c # positive side collision
        return (self.c + self.r) >= (other.c - other.r)
      else  # self.c > other.c, so negative side collision
        return (self.c - self.r) <= (other.c + other.r)
      end
    end
    
    # Calculates bump time for collisions on the positive side of self
    def positive_bump_time(other)
      # Δtᵇ is the time of collision calculated from now, as we define t⁰ == 0.
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
      vrel = self.v - other.v # Relative speed.
      crel = self.c - other.c # Relative position.
      rsum = self.r + other.r # Sum of the radiuses.
      tb   = (rsum - crel) / vrel
      return tb      
    end
    
    
    # Calculates bump time for collisions on the negative side of self
    def negative_bump_time(other)
      # Δtᵇ is the time of collision calculated from now, as we define t⁰ == 0.
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
      vrel = self.v - other.v # Relative speed.
      crel = self.c - other.c # Relative position.
      rsum = self.r + other.r # Sum of the radiuses.
      tb   = (-(rsum + crel)) / vrel
      return tb
    end
    
    
    # Calculates the time when self will bump into other, or nil if 
    # their speeds are the same in which case they will either never collide, 
    # or always were and always will be colliding, assuming their speed does 
    # not change.     
    # If the bump time is negative, the initial collision took place in the 
    # past, and may still be ungoing. 
    def bump_time(other)
      return nil if self.v == other.v
      # Two objects moving at equal speed who are not already colliding
      # will never collide with each other.
      if self.c <= other.c 
        # The collision will happen or has happened on the positive 
        # side of self at self.c + self.r and at the negative side of 
        # other at other.c - other.r
        return negative_bump_time(other)
      else # here self.c > other.c
        # The collision will happen or has happened on the negative 
        # side of self at self.c - self.r and at the positive side of 
        # other at other.c + other.r
        return positive_bump_time(other)
      end
    end
    
    # Caluclates the prediced position of c where self will bump into other
    # if both retain the same speeds. Returns nil if their speeds are the same 
    # in which case they will either never collide, or always stay in overlap.  
    def bump_c(other)
      tb     = bump_time(other) 
      return nil unless tb
      # self.cᵇ          == self.v * tᵇ + self.c⁰ (3)
      cb     = self.v * tb + self.c
      return cb
    end
    
    # Simulates a motion that takes place during a time interval dt (Δt)
    # assuming the speed remained constant during that interval.
    # Returns a new line piece aline with it's c value updated to reflect 
    # the changed position.
    # @return [Aline] 
    def step(dt=1.0)
      dc    = dt * self.v
      newc  = self.c + dc
      return self.class.new(newc, self.r, self.v)
    end
    
    # Simulates a motion as per #step, but modifies self. 
    # @return [Aline]
    def step!(dt=1.0)
      dc    = dt * self.v
      @c    = self.c + dc
      return self
    end
        
    
  end # class Aline
end # module Bump      
    
    
  
  
  