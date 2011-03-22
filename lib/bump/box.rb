module Bump
  # A Box is an axis-oriented bounds box.
  class Box
    # include Mobile
    include Thing
    
    attr_reader :v
    attr_reader :dp
    
    def initialize(opt={})
      super(opt)
      vx  = opt[:vx] || 0.0
      vy  = opt[:vx] || 0.0
      @v  = opt[:v]  || Bump.vec(vx, vy)
      @dp = Bump.vec(0.0, 0.0)
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
    
    # Returns the position of self after collosion impulses have been added
    def q
      return @p + @dp
    end
    
    # Returns the x position of self after cillosion impulses have been added
    def qx
      return self.q.x
    end
    
    # Returns the y position of self after cillosion impulses have been added
    def qy
      return self.q.y
    end
    
    
    
    def bump_now_x?(other)
      if self.qx < other.qx # positive side collision
        return (self.qx + self.rx) >= (other.qx - other.rx)
      else  # self.px > other.px, so negative side collision
        return (self.qx - self.rx) <= (other.qx + other.rx)
      end
    end
    
    def bump_now_y?(other)
      if self.qy < other.qy # positive side collision
        return (self.qy + self.ry) >= (other.qy - other.ry)
      else  # self.px > other.px, so negative side collision
        return (self.qy - self.ry) <= (other.qy + other.ry)
      end
    end
    
    
    # Returns true if self is overlapping with other right now, 
    # at the current position, fale if not 
    def bump_now?(other)      
      return true if self.q == other.q
      return bump_now_x?(other) && bump_now_y?(other)
    end
    
    # Calculates bump time for collisions on the positive side of self
    # for x and y. Other must be a Box.
    def positive_bump_time(other)
      vrel = self.v  - other.v # Relative speed.
      crel = self.q  - other.q # Relative position.
      rsum = self.r  + other.r # Sum of the radiuses.
      tbx  = (vrel.x != 0.0 ? (rsum.x - crel.x) / vrel.x : nil)
      tby  = (vrel.y != 0.0 ? (rsum.y - crel.y) / vrel.y : nil)
      return tbx, tby
    end
    
    # Calculates bump time for collisions on the negative side of self
    # for x and y. Other must be a Box.
    def negative_bump_time(other)
      vrel = self.v - other.v # Relative speed.
      crel = self.q - other.q # Relative position.
      rsum = self.r + other.r # Sum of the radiuses.
      tbx  = (vrel.x != 0.0 ? (-(rsum.x + crel.x)) / vrel.x : nil)
      tby  = (vrel.y != 0.0 ? (-(rsum.y + crel.y)) / vrel.y : nil)
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
      bx       = self.qx < other.qx  ? nbx : pbx
      by       = self.qy < other.qy  ? nby : pby
      return bx, by
    end
    
    def bump_time(other)
      nowy = bump_now_y?(other)
      nowx = bump_now_x?(other)
      return 0 if nowy && nowx
      bx, by = bump_times(other)
      # If the boxes already overlaping in one direction, the other direction 
      # is determining
      return bx if nowy
      return by if nowx
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
      pb      = self.v * tb + self.q
      return pb
    end
    
    # Calculates the position of self after dt
    def p_after(dt=1.0)
      return (self.v * dt) + self.q
    end
      
    # Calculates the change in position of self after dt, collision impulse 
    # included
    def dp_after(dt=1.0)
      return (self.v * dt) + self.dp
    end  


    # Simulates a motion that takes place during a time interval dt (Δt)
    # assuming the speed remained constant during that interval.
    # Returns a new line piece aline with it's p value updated to reflect 
    # the changed position.
    # @return [Aline] 
    def step(dt=1.0)
      deltap  = self.v * dt
      newp    = self.q + deltap
      return self.class.new(:p => newp, :r => self.r, :v => self.v)
    end
    
    # Simulates a motion as per #step, but modifies self. 
    # @return [Aline]
    def step!(dt=1.0)
      deltap  = self.v * dt
      @p      = self.q + deltap      
      reset_impulse
      return self
    end
    
    # Adds the given vector value to this object's collision motion
    def impulse(ddp)
      @dp = @dp + ddp
    end
    
    # Resets the collision motion to 0 
    def reset_impulse()
      @dp   = Bump.vec(0.0, 0.0)
    end   
    
    # Checks if self will bump into other after dt has passed. 
    # It returns 0.0 if already wholly colliding. 
    # Returns the bump time if it is less than dt, otherwise returns nil.
    def bump_after?(other, dt=1.0)
      nowy = bump_now_y?(other)
      nowx = bump_now_x?(other)
      return 0.0 if nowy && nowx
      bx, by = bump_times(other)
      return bx if nowy && bx && bx >= 0.0 && bx <= dt 
      return by if nowx && by && by >= 0.0 && by <= dt
      return bx if bx && by && by <= bx && bx <= dt
      return by if by && bx && bx <= by && by <= dt
      return nil
    end
    
    # Handles collision with another box for the step dt.
    # TODO: 
    # * Fix stickyness of collision, probably due to bounds misinterpretation.
    # * Fix multi-box collisions which someties don't work. 
    def bump_with(other, dt=1.0, debug = false)
      bt      = bump_after?(other, dt)
      return nil unless bt
      # Respective motions after collision time. 
      spbump  = dp_after(bt)
      opbump  = other.dp_after(bt)
       # Position changes that self would have gone through.
      pself   = dp_after(dt)
      # Position changes that other would have gone through.       
      pother  = other.dp_after(dt)
      # Difference of what would have happened and what have happend must be 
      # applied reciprocally to colliding object.
      dps     = pself  - spbump
      dpo     = pother - opbump 
      if debug
      puts "Bump #{dps} #{dpo} #{pself} #{pother} #{spbump} #{opbump}."
      end
      self.impulse(dpo)
      other.impulse(dps)      
    end
    
  end
end