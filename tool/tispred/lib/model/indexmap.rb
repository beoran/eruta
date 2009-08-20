# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 
#
# Model for an indexed bitmap.
#

require 'model/palette'

class Indexmap
  attr_reader :wide
  attr_reader :high

  def initialize(wide, high, palette)
    @wide     = wide
    @high     = high
    @palette  = palette
    @pixels   = Array.new(high)
    @lookup   = Array.new(high)
    for ii in 0...@high do
      @pixels[ii] = Array.new(wide, Rgb::TRANSPARENT)
      @lookup[ii] = Array.new(wide, Palette::TRANSPARENT)
    end
  end
  
  def each_rgba(&block)
   for y in 0...@high
     row = @pixels[y]
     for x in 0...@wide
       color = row[x]
       block.call(x, y, color.r, color.g, color.b, color.a)
     end
   end
  end
  
  def fill(pal_index = Palette::TRANSPARENT)
    self.each_rgba { |x, y, r, g ,b, a| set(x, y, pal_index)  }
  end
  
  
  def set(x, y, pal_index = Palette::TRANSPARENT)
    @lookup[y][x] = pal_index
    @pixels[y][x] = @palette[pal_index]
  end
  
  def get(x, y)
    return @lookup[y][x]
  end
  
  def get_color(x, y)
    return @pixels[y][x]
  end
  
  def get_pixel(x, y)
    color = self.get_color(x, y)
    return color.r, color.g, color.b, color.a
  end
  
  def mouse_pressed(lx, ly, x, y)
    
  end
  
  def mouse_released(lx, ly, x, y)
    
  end
  
  def mouse_dragged(lx, ly, x, y)
    
  end
  
  

  def draw_line(x1, y1, x2, y2, pal_index)
   dx = (x2 - x1).abs
   dy = (y2 - y1).abs
   x = x1
   y = y1

   if (x1 > x2) then 
     offsetx = -1 
   else 
     offsetx = 1 
   end

   if (y1 > y2) then 
     offsety = -1 
   else 
     offsety = 1
   end

   self.set(x,y, pal_index)

   if (dx > dy) then
     error = dx / 2
     while (x != x2)
       error = error - dy
       if (error < 0)
         y = y + offsetx
         error = error + dx
       end
       x = x + offsety
       self.set(x,y, pal_index)
     end
   else
     error = dx / 2
     while (y != y2)
       error = error - dy
       if (error < 0)
         x = x + offsetx
         error = error + dy
       end
       y = y + offsety
       self.set(x,y, pal_index)
       end
     end
  end 

=begin  
  void rasterCircle(int x0, int y0, int radius)
  {
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;
 
    setPixel(x0, y0 + radius);
    setPixel(x0, y0 - radius);
    setPixel(x0 + radius, y0);
    setPixel(x0 - radius, y0);
 
    while(x < y) 
    {
      if(f >= 0) 
      {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x + 1;    
      setPixel(x0 + x, y0 + y);
      setPixel(x0 - x, y0 + y);
      setPixel(x0 + x, y0 - y);
      setPixel(x0 - x, y0 - y);
      setPixel(x0 + y, y0 + x);
      setPixel(x0 - y, y0 + x);
      setPixel(x0 + y, y0 - x);
      setPixel(x0 - y, y0 - x);
    }
  }
=end
  
  
  def line(x, y , wide, high, pal_index)
    
  end
  
  def rectfill(x, y, wide, high, pal_index)
    xstop = x + wide
    ystop = y + high
    for ydex in y..ystop do
      for xdex in x..xstop do
        self.set(xdex, ydex, pal_index)
      end 
    end    
  end
  
  def clear(pal_index)
    for ydex in 0..high do
      for xdex in 0..wide do
        self.set(xdex, ydex, pal_index)
      end 
    end
  end
  
  
end
