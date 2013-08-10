module Zori
  class Rect
    attr_accessor :x
    attr_accessor :y
    attr_accessor :h
    attr_accessor :w
    def initialize(x, y, h, w)
      @x, @y, @h, @w = x , y, h, w
    end 
  end
end
