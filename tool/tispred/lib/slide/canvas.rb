# Type of panel in which an image can be edited in a grid. 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 
require 'slide'
require 'swing'
require 'awt'

module Slide
  

class Canvas
  include Slide::Wrapper
  
  class Drawpanel < Swing::JPanel
  
  # include Awt::MouseMotionListener 
  # include Awt::MouseListener  
  
  def initialize(clip = nil)
      super()
      @gridsize   = 5 # first set the grid size, since set clip needs it!!!      
      self.clip   = clip            
      @lastpos    = Awt::Point.new(0,0)
      @gridcolor  = Awt::Color.new(128,128,128,255) 
  end

  def clip=(clip)
    @clip = clip
    if @clip
      @wide = clip.wide
      @high = clip.high
      self.setSize(Awt::Dimension.new(@wide * @gridsize, @high * @gridsize))
      self.setPreferredSize(Awt::Dimension.new(@wide * @gridsize, @high * @gridsize))
      self.setMaximumSize(Awt::Dimension.new(@wide * @gridsize, @high * @gridsize))
    end    
  end
  
  # Paints the clip if it's not nil
  def paint_clip(gfx)        
    y = 0
    for ydex in 0...@high
      x = 0      
      for xdex in 0...@wide
        r, g, b, a = @clip.get_pixel(xdex, ydex)
        if a > 0
          color = Awt::Color.new(r, g, b, a)
          gfx.setColor(color)
          gfx.fillRect(x, y , @gridsize, @gridsize)
        end
        gfx.setColor(@gridcolor)
        gfx.drawRect(x, y , @gridsize, @gridsize)
        x += @gridsize
      end
      y += @gridsize
    end
  end
  
  def paintComponent(g)
    paintChildren(g) if self.respond_to? :paintChildren 
    return unless @clip
    paint_clip(g)
  end  
    
 
     
  end
  
  def initialize(clip=nil)
    @swing = Drawpanel.new(clip)
    self.enable_mouse
    self.mouse.click do | b, x, y |
      @clip.click(b, x, y) if @clip.respond_to? :click
      puts "Clicked #{b} #{x} #{y}"
    end
    self.mouse.drag do | b, x, y |
      @clip.drag(b, x, y)  if @clip.respond_to? :drag
      puts "Dragged #{b} #{x} #{y}"
    end
  end
  
  
  
end # class Canvas

end # module Slide