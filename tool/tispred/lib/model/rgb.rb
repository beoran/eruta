# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 

class Rgb
  attr_reader :r
  attr_reader :g
  attr_reader :b
  attr_reader :a
  
  # Creates new color based on RGBA Model
  def initialize(*args)
    @r    = args[0]
    @g    = args[1]
    @b    = args[2]
    @a    = args[3] || 0 # Solid by default
    @name = args[4] || "None"
  end
  
  TRANSPARENT = Rgb.new(255,0,255,255, "Transparent") # Transparent color. 
  
  def to_ary
    return [ @r, @b, @g, @a ]
  end
  
  def to_splat
    return @r, @b, @g, @a
  end
  
end
