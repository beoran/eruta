# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 
require 'model/rgb'

class Palette
  
  TRANSPARENT = 0
  
  def initialize
    @colors               = []
    @colors[TRANSPARENT]  = Rgb::TRANSPARENT
    @colors[1]            = Rgb.new(200, 100, 50, 255)
  end
  
  def << (color)
    @colors << color
  end
  
  def size
    return @colors.size
  end
  
  def [] (index)
    return @colors[index]
  end
  
  def []= (index, value)
    return @colors[index] = value
  end
  
  def delete(index)
    @colors.delete index
  end
  
  def action(index)
    color = @colors[ineex]
    puts "Action for color #{color}"
  end
  
  def add_rgba(r, g, b, a)
    color = Rgb.new(200, 100, 50, 225)
    self << color
  end
  
  def change_rgba(index, r, g, b, a)
    color       = Rgb.new(r,g,b,a)
    self[index] = color
  end
  
  def get_rgba(index)
    color = self[index]
    return color.r, color.g, color.b, color.a
  end
  
end
