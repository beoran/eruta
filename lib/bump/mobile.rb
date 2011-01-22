module Bump
  # Anything that can move with regards to collision detection.
  module Mobile
    include Thing
    attr_accessor :dp # distance vector that the center will be moved by 
    # in the next time step dt
    attr_accessor :v  # speed vector
    attr_accessor :m  # mass
    
    def initialize(opt={})
      super(opt)
      @v  = Bump.vec(opt[:vx] || 0.0, opt[:vy] || 0.0)
      @dp = Bump.vec0
      @m  = opt[:m] || 1.0
    end
    
    
    def prepare_p(dt)
      @dp = (@v * dt)
    end
    
    def update_p
      @p += @dp
    end
   
    def prepare(dt)
      prepare_p dt
    end
    
    def update(dt)
      update_p
    end
 
    # relative speed towards other mobile
    def vrel(other)
      return self.v - other.v
    end
    
    # The new x position the bounding box of the mobile will have after moving
    # dp to it's new position
    def newx
      return self.x + self.dp.x
    end
    
    # The new y position the bounding box of the mobile will have after moving
    # dp to it's new position
    def newy
      return self.y + self.dp.y
    end
    
    
    # returns true if the bounding box of the other 
    # will collide (dp is taken into account)
    def will_collide?(other)
      return false if other.newx > self.newx  + self.w 
      return false if self.newx  > other.newx + other.w
      return false if other.newy > self.newy  + self.h 
      return false if self.newy  > other.newy + other.h
      return true
    end
    
    # Collide with all possible mobile objects
    def collide_mobiles(dt, others)
      others.each do |other|
        self.collide_mobile(dt, other)
      end
    end
    
    def dpx=(xx)
      self.dp = Bump.vec( xx, self.dp.y)
    end

    def dpy=(yy)
      self.dp = Bump.vec(self.dp.y, yy)
    end

    def calculate_mobile_collision_x(dt, other) 
      if other.dp.x == 0.0 
        if self.dp.x == 0.0
          return
        else  
          return other.calculate_mobile_collision_x(self)
        end
      end  
      xrat        = self.dp.x / other.dp.x
      # other is pushed back (1 - xrat) * other.dp.x , self xrat * other.dp.x
      self.dpx    = self.dp.x - (xrat * self.dp.x)
      other.dpx   = self.dp.x - ((1.0 - xrat) * other.dp.x)     
    end

    def calculate_mobile_collision_y(dt, other) 
      if other.dp.y == 0.0 
        if self.dp.y == 0.0
          return
        else  
          return other.calculate_mobile_collision_y(self)
        end
      end  
      yrat        = self.dp.y / other.dp.y
      # other is pushed back (1 - yrat) * other.dp.y , self xrat * other.dp.y
      self.dpy    = self.dp.y - (yrat * self.dp.y)
      other.dpy   = self.dp.y - ((1.0 - yrat) * other.dp.y)     
    end

    def calculate_mobile_collision(dt, other) 
      calculate_mobile_collision_x(dt, other) 
      calculate_mobile_collision_y(dt, other)
    end
 
    # Prepare a collision with the other mobile object
    def collide_mobile(dt, other)
      return nil if self == other
      # FIXME: Of course this is clearny NOT swept collision detection, merely
      # simple collision detection. I implement this temorarily like this
      # to gain confindence and insight in tackling the more difficult problem
      return nil unless self.will_collide?(other)
      # we are colliding right now
      # calculate the relative displacement needed by self and other
      if self.dp.zero? && other.dp.zero? 
        # two mobiles are colliding, but the dp is zero. 
        # this could only happen if the objects were placed in an overlapping
        # position by direct manipiulation of p
        # nothing much we can do here but warn?
        warn "Objects stuck into each other"
      else
        self.calculate_mobile_collision(dt, other)
      end
    end
    
  
  end
end