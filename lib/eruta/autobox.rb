

#!../ext/bin/ruby
#
# Copyright Bjorn De Meyer 2007. Can be used freely under the zlib license.
#
#

BEGIN { $:.unshift(File.dirname(__FILE__)); }
# Include this script's directory in Ruby's search path.

require 'rubygame'
require 'rubygame_ttf'
require 'tilemap'
require 'utility'
require 'rogaru/image'


Rubygame.init()
END { Rubygame.quit() }

def surface_is_transparent(surf, x, y)
    res = surf.get_at(x,y)
  r,g,b,a = *res
  return a < 255
end

class Point
  attr_accessor :x
  attr_accessor :y
  def initialize(x, y)
    @x = x
    @y = y
  end
  def <=>(other)
    xdiff = other.x**2 <=> @x**2
    return other.y**2 <=> @y**2 if xdiff == 0
    return xdiff 
  end  
end


class Line
  attr_accessor :x1
  attr_accessor :y1
  attr_accessor :x2
  attr_accessor :y2
  def initialize(x1, y1, x2, y2)
    @x1 = x1
    @y1 = y1
    @x2 = x2
    @y2 = y2
  end
  
  def slope
    return (y2-y1).to_f / (x2-x1).to_f
  end  
  
  def dist
    return ((x2-x1)**2 + (y2-y1)**2)**0.5
  end    
  
  def each(&block)
    x = @x1 < @x2 ? @x1 : @x2
    y = @y1 < @y2 ? @y1 : @y2
    d = self.dist
    xstop  = @x1 < @x2 ? @x2 : @x1
    ystop  = @y1 < @y2 ? @y2 : @y1
    while x <= xstop
      y =  ((x - @x1 ) *  self.slope + @y1).floor
      yield x , y  
      x += 1.0 / dist
    end
    
  end

  def draw(screen)
    self.each {   
      |x,y| screen.draw_box([x,y], [x,y], [255,255,255]) 
  } 
  end

  def intersects(surface)
    self.each do |x, y|
    begin
     return true unless surface_is_transparent(surface, x,y)
    rescue IndexError  
    # Don't care
    end
    end
    return false
    end    

end


class Surfrect
  attr_accessor :x1
  attr_accessor :y1
  attr_accessor :x2
  attr_accessor :y2

  def empty
    return (((@x2-@x1).abs < 1) or ((@y2-@y1).abs < 1))
  end    

  def initialize(x1, y1, x2, y2)
    @x1 = x1
    @y1 = y1
    @x2 = x2
    @y2 = y2
  end

  def self.from_surface(surf)
    return self.new(0, 0, surf.w, surf.h)
  end
    
  def to_s
    return "Surfrect #{x1},#{y1} -> #{x2},#{y1}"
  end
end

def is_transparent(surface, x, y)
    res = surface.get_at(x,y)
  r,g,b,a = *res
  return a < 255
end


def is_transparent_vline(surface, x, ystart, ystop)
  for y in (ystart..ystop) do
    return false unless is_transparent(surface, x ,y)
  end
  return true
end

def is_transparent_hline(surface, y, xstart, xstop)
  for x in (xstart..xstop) do
    return false unless is_transparent(surface, x ,y)
  end
  return true
end



class Bound

  attr_accessor :rect

  def initialize(surface, rect) 
    @surface     = surface
    @rect       = rect
    @points     = []
    @map       = self.make_solid_map
    self.points_from_map(@map)
    @count = @points.size
    target = 64.0
    skip   = ( @count / target ).floor 
    skip = 2 if skip < 2
    puts skip.to_s
                @allpoints = @points.sort
    # @allpoints = simplify( @points.sort, skip )
    @allsize   = @allpoints.size
    puts "Total points x, y, total: #{@count} #{@allsize}"
  end

  def simplify(points, skip=1)
    result = []
    skip = 1 if skip < 1
    index = 0
    stop = points.size
    while index < stop 
      result.push(points[index])
      index += skip
    end
    return result;
  end    

  def make_solid_map()
    result = []
    for x in (@rect.x1..@rect.x2) do
      result[x] = []
      for y in (@rect.y1..@rect.y2) do
        result[x][y] = (not(surface_is_transparent(@surface, x, y))) 
      end
    end
    return result
  end
  
  def point_is_outline(x, y, map)
    here   = map[x][y]
    if x > @rect.x1 then
      left   = map[x-1][y]
    else
      left = false
    end
    if x < @rect.x2 then
      right   = map[x+1][y]
    else
      right = false
    end
    if y > @rect.y1 then
      top   = map[x][y-1]
    else
      top     = false
    end
    if y < @rect.y2 then
      bottom   = map[x][y+1]
    else
      bottom  = false
    end
# and (not(left or right or top or bottom))
    return (here and ( (not(left and right and top and bottom)) ))
  end

  def points_from_map(map)
    @points = []
    for x in (@rect.x1..@rect.x2) do
      for y in (@rect.y1..@rect.y2) do
        if point_is_outline(x, y , map) then
          @points.push(Point.new(x, y))
        end
      end
    end
  end
        
  def first_outline_point(map)    
    for x in (@rect.x1..@rect.x2) do
      for y in (@rect.y1..@rect.y2) do
        if point_is_outline(x, y , map) then
          return Point.new(x,y)
        end
      end
    end
  end
  
  
  def calculate_points_x
    outside = true
    x     = @rect.x1
    while x < @rect.x2 do
      @xpoints[x] = []
      for y in (@rect.y1...@rect.y2) do
        begin
        trans = surface_is_transparent(@surface, x, y)
        if outside and (! trans)
          outside = false
          @xpoints[x].push(Point.new(x , y-1))
          # ray is cutting the object
        elsif (!outside) and trans
          @xpoints[x].push(Point.new(x, y -1))
          # ray is cutting the object
          outside = true
        end  
        rescue IndexError
        end
      end
      x+= @xstep      
    end
  end

  def calculate_points_y
    outside = true
    y     = @rect.y1
    while y < @rect.y2 do
      @ypoints[y] = []
      for x in (@rect.x1...@rect.x2) do
        begin
        trans = surface_is_transparent(@surface, x, y)
        if outside and (! trans)
          outside = false
          @ypoints[y].push(Point.new(x-1 , y))
          # ray is cutting the object
        elsif (!outside) and trans
          @ypoints[y].push(Point.new(x-1, y))
          # ray is cutting the object
          outside = true
        end  
        rescue IndexError
        end
      end  
      y+= @ystep    
    end
  end


  def draw(screen)
    @acolor = [0,0,0] if @acolor.nil?
    @allpoints.each do |point|
      next if point.nil? 
       screen.draw_box([point.x,point.y], [point.x+1, point.y+1], @acolor) 
    end

  end  
end



def main(filename = nil, fullscreen = false)
  queue     = Utility.event_setup()
  screen    = Utility.screen_make(640, 480, fullscreen, true)
  
  clock           = Rubygame::Clock.new()
  ttf     = Rubygame::TTF
  ttf.setup()
  font     = ttf.new("../share/font/crystal.ttf", 16)
  filename =  "../share/image/sprite_greenghost.png" if filename.nil? or filename.empty?
  image = Rogaru::Image.load_alpha(filename);
  # puts image.pixels
  rect  = Surfrect.from_surface image
  a = Time.now
  bound = Bound.new image, rect
  b = Time.now
  puts "#{b-a}"
  puts rect.to_s
  lin   = Line.new(99, 101, 78, 502)
  
  puts "inter: #{lin.intersects image}"  

  done= false
  while not done do
    screen.fill [ 100, 100, 100] ;
    image.blit(screen, [0, 0]) 
    bound.draw(screen)  
    screen.flip
    queue.each do   
      | event | 
      case event 
      when Rubygame::KeyDownEvent then 
        puts "#{event.key}"
        done = true if event.key == Rubygame::K_F10
      when Rubygame::QuitEvent then
        done = true
      end
    end

  end

end

main ARGV[0]
