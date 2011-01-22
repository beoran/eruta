module Bump
  # A Box is an axis-oriented bounds box.
  class Box
    include Mobile
    
    attr_reader :w 
    attr_reader :h 
    
    def initialize(opts={})
      super(opts)
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
end