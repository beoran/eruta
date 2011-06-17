module Bump
  # A thing is anything that can be used in the bump system as a 
  # colliding object.
  module Thing
    # Position of the center of the object.
    attr_reader :p
    # Width of the bounding box of object.
    attr_reader :w
    # Height of the bounding box of object.
    attr_reader :h
    # Radius pointer, describes size from the center of object p.
    attr_reader :r
    
    def initialize(opt={})
      @w  = opt[:w]  || opt[:rx] * 2 
      @h  = opt[:h]  || opt[:ry] * 2
      rx  = opt[:rx] || @w / 2.0
      ry  = opt[:rx] || @h / 2.0
      @r  = opt[:r]  || Bump.vec(rx, ry)
      px  = opt[:px] ? opt[:px] : opt[:x] + @r.x
      py  = opt[:py] ? opt[:py] : opt[:y] + @r.y 
      @p  = opt[:p] || Bump.vec(px, py)
    end
   
    # x coordinate of the left of the thing's bounding box
    def x
      (@p.x - @r.x).to_i
    end
    
    # y coordinate of the top of the thing's bounding box
    def y
      (@p.y - @r.y).to_i
    end
    
    # x coordinate of mobile's center
    def px
      return self.p.x
    end
    
    # y coordinate of mobile's center
    def py
      return self.p.y
    end
    
    # X radius
    def rx
      return self.r.x
    end
    
    # Y radius
    def ry
      return self.r.y
    end
    
    
  end
end