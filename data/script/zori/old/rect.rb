module Zori
  class Rect
    attr_accessor :x
    attr_accessor :y
    attr_accessor :h
    attr_accessor :w
    
    def initialize(x, y, w, h)
      @x, @y, @w, @h = x , y, w, h
    end 
    
    # Checks if the coordinates are inside the rectangle.
    def inside?(x, y)
      return (x >= @x) && (x <= (@x + w)) && (y >= @y) && (y <= (@y + h))
    end
    
  end
end
