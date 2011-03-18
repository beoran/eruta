module Bump
  # A Box is an axis-oriented bounds box.
  class Box
    # include Mobile
    include Thing
    
    attr_reader :v
    
    def initialize(opt={})
      super(opt)
      vx = opt[:vx] || 0.0
      vy = opt[:vx] || 0.0
      @v = opt[:v]  || Bump.vec(vx, vy)
    end
    
    def vx
      @v.x
    end
      
    def vy
      @v.y
    end
    
    def vx=(xv)
      @v = Bump.vec(xv, self.vy) 
    end
    
    def vy=(yv)
      @v = Bump.vec(self.vx, yv) 
    end
    
    def bump_now_x?(other)
      if self.px < other.px # positive side collision
        return (self.px + self.rx) >= (other.px - other.rx)
      else  # self.px > other.px, so negative side collision
        return (self.px - self.rx) <= (other.px + other.rx)
      end
    end
    
    def bump_now_y?(other)
      if self.py < other.py # positive side collision
        return (self.py + self.ry) >= (other.py - other.ry)
      else  # self.px > other.px, so negative side collision
        return (self.py - self.ry) <= (other.py + other.ry)
      end
    end
    
    
    
    # Returns true if self is overlapping with other right now, 
    # at the current position, fale if not 
    def bump_now?(other)      
      return true if self.p == other.p
      return bump_now_x?(other) && bump_now_y?(other)
    end
    
    # Calculates bump time for collisions on the positive side of self
    # for x and y. Other must be a Box.
    def positive_bump_time(other)
      vrel = self.v - other.v # Relative speed.
      crel = self.p - other.p # Relative position.
      rsum = self.r + other.r # Sum of the radiuses.
      tbx  = vrel.x ? (rsum.x - crel.x) / vrel.x : nil
      tby  = vrel.y ? (rsum.y - crel.y) / vrel.y : nil
      return tbx, tby
    end
    
    # Calculates bump time for collisions on the negative side of self
    # for x and y. Other must be a Box.
    def negative_bump_time(other)
      vrel = self.v - other.v # Relative speed.
      crel = self.p - other.p # Relative position.
      rsum = self.r + other.r # Sum of the radiuses.
      tbx  = vrel.x ? (-(rsum.x + crel.x)) / vrel.x : nil
      tby  = vrel.y ? (-(rsum.y + crel.y)) / vrel.y : nil
      return tbx, tby
    end
    
    # Calculates the times when self will bump into other, or nil if 
    # their speeds are the same in which case they will either never collide, 
    # or always were and always will be colliding, assuming their speed does 
    # not change.
    # If the bump time is negative, the initial collision took place in the 
    # past, and may still be ungoing. 
    def bump_times(other)
      return nil, nil if self.v == other.v
      # Two objects moving at equal speed who are not already colliding
      # will never collide with each other.
      nbx, nby = negative_bump_time(other)
      pbx, pby = positive_bump_time(other)
      bx       = self.px < other.px  ? nbx : pbx
      by       = self.py < other.py  ? nby : pby
      return bx, by
    end
    
    def bump_time(other)
      bx, by = bump_times(other)
#       return nil, :none , nil unless bx && by 
#       return bx , :x    , by if bx < by 
#       return by , :y    , bx
      return nil  unless bx && by 
      return bx   if bx < by && bx >= 0.0 
      return by    
    end  
    
    # Caluclates the prediced position p where self will bump into other
    # if both retain the same speeds. Returns nil if their speeds are the same 
    # in which case they will either never collide, or always stay in overlap.  
    def bump_p(other)
      tb      = bump_time(other) 
      return nil unless tb
      pb     = self.v * tb + self.p
      return pb
    end
    
    # Simulates a motion that takes place during a time interval dt (Δt)
    # assuming the speed remained constant during that interval.
    # Returns a new line piece aline with it's p value updated to reflect 
    # the changed position.
    # @return [Aline] 
    def step(dt=1.0)
      dp    = self.v * dt
      newp  = self.p + dp
      return self.class.new(:p => newp, :r => self.r, :v => self.v)
    end
    
    # Simulates a motion as per #step, but modifies self. 
    # @return [Aline]
    def step!(dt=1.0)
      dp    = self.v * dt
      @p    = self.p + dp
      return self
    end
    
    
    
  end
end