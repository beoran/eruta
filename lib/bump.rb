#
# Bump, a module for swept collision detection.
#
#
require 'gari'

module Bump
  def self.vec(x, y)
    Gari.vec(x, y)
  end
  
  def self.vec0
  
    Gari::Vector.zero
  end
  
  
  # anything that can move
  module Mobile
    attr_reader   
    
    attr_accessor :p  # position of the center of the mobile
    attr_accessor :dp # distance that the cengter will be moved by 
    # in the next time step dt
    attr_accessor :v  # speed vector
    attr_accessor :m  # mass
    
    def initialize(opt={})
      @v = Bump.vec(opt[:vx] || 0.0, opt[:vy] || 0.0)
      @dp= Bump.vec0
      @m = opt[:m] || 1.0
      @p = Bump.vec(opt[:px] || 0.0, opt[:py] || 0.0)
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
    
    def update
      update_p
    end
 
    # relative speed towards other mobile
    def vrel(other)
      return self.v - other.v
    end
    
    # x coordinate of mobile's center
    def px
      return self.p.x
    end
    
    # y coordinate of mobile's center
    def py
      return self.p.y
    end


    
  end

  # A Box is an axis-oriented bounds box.
  class Box
    include Mobile
    
    attr_reader :w 
    attr_reader :h 
    
    def initialize(x, y, w, h)
      super(:px => x + w / 2.0 , :py => y + h / 2.0 )
      @w  = w
      @h  = h
    end
  
    # x coordinate of mobile's upper left corner, for drawing
    # or blitting
    def dx
      return (self.px - (self.w / 2)).to_i
    end

    # x coordinate of mobile's upper left corner, for drawing
    # or blitting
    def dy
      return (self.py - (self.h / 2)).to_i
    end
 
    # time when this box would collide with another on the right side 
    # of this box. Returns nil if the collision would not happen, or 
    # a negative time if it would happen in the past.
    def collide_t_right(other) 
      vrx = other.v.x - self.v.x
      return nil if vrx == (0.0)
      return (self.p.x - other.px + self.w) / vrx
    end

    # time when this box would collide with another on the left side 
    # of this box. Returns nil if the collision would not happen, or 
    # a negative time if it would happen in the past.
    def collide_t_left(other)
      return other.collide_t_right(self)
    end
    
    # time when this box would collide with another on the bottom side 
    # of this box. Returns nil if the collision would not happen, or 
    # a negative time if it would happen in the past.
    def collide_t_bottom(other) 
      vr = self.vrel(other)
      return nil if vr.y == (0.0)
      return (other.p.y - self.p.y - self.h) / vr.y
    end
    
    # time when this box would collide with another on the top side 
    # of this box. Returns nil if the collision would not happen, or 
    # a negative time if it would happen in the past.
    def collide_t_top(other) 
      vr = other.vrel(self)
      return nil if vr.y == 0.0
      return (self.p.y - other.p.y - other.h) / vr.y
    end
   
    # Time at which this box would collide with the other.  
    def collide_t(other)
      ctr = self.collide_t_right(other)
      ctl = self.collide_t_left(other)
      return ctl, ctr
      # meaning of ctl (collision time left) and ctr (collision time right)
      # ctl and ctr are negative: collision in the past, so no collision
      # ctr and ctl are positive: collision, smallest value first
      # opposite sign: currently colliding, positive val is exit time
      return nil unless ctr || ctl
      return ctr unless ctl
      return ctl unless ctr
    end
    
    def collide_now?(other)
      return false if other.dx > self.dx  + self.w 
      return false if self.dx  > other.dx + other.w
      return false if other.dy > self.dy  + self.h 
      return false if self.dy  > other.dy + other.h
      return true
    end
    
    
    
  end

  autoload :Lookup, 'bump/lookup'
  autoload :Space , 'bump/space'

end