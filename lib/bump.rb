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
    
    attr_accessor :p # position of the center of the mobile
    attr_accessor :v # speed vector
    attr_accessor :a # accelleration vector
    attr_accessor :m # mass
    attr_accessor :f # force vector
    
    def initialize
      @f = Bump.vec0
      @a = Bump.vec0
      @v = Bump.vec0
      @m = 1.0
      @p = Bump.vec0
    end
    
    def update_a(dt)
      return if @f.zero? || @m == 0.0
      @a += (@f * dt) / @m
    end

    def update_v(dt)
      return if @a.zero?
      @v += (@a * dt)
    end
    
    def update_p(dt)
      return if @v.zero?
      @p += (@v * dt)
    end
   
    def update(dt)
      update_a dt
      update_v dt
      update_p dt
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
    
    attr_reader :wv # half width of the box as a vector
    attr_reader :hv # half height of the box as a vector
    
    def initialize(x, y, w, h)
      super()
      w2  = w / 2.0
      h2  = h / 2.0
      @p  = Bump.vec(x + w2, y + h2)
      @wv = Bump.vec(w2    , 0.0   )
      @hv = Bump.vec(0.0   , h2    )
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

    def w
      return (@wv + @wv).x.to_i 
    end
    
    def h
      return (@hv + @hv).y.to_i 
    end
    
    # time when this box would collide with another on the right side 
    # of this box. Returns nil if the collision would not happen, or 
    # a negative time if it would happen in the past.
    def collide_t_right(other) 
      vr = self.vrel(other)
      return nil if vr.x == (0.0)
      return (other.p.x - self.p.x - self.w) / vr.x
    end

    # time when this box would collide with another on the left side 
    # of this box. Returns nil if the collision would not happen, or 
    # a negative time if it would happen in the past.
    def collide_t_left(other)
      vr = other.vrel(self)
      return nil if vr.x == 0.0
      return (self.p.x - other.p.x - other.w) / vr.x
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
      acts = [:collide_t_top, :collide_t_right,
             :collide_t_bottom, :collide_t_left]
      aid = []       
      for act in acts do
        res = self.send(act, other)
        aid << [ res, act ] if res && res >= 0.0 
      end
      res = aid.sort.first
      return nil, :none unless res
      return * res
    end
  end


end