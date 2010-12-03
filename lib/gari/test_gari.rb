#
#
#

$: << '.'
require 'gari.so'
require 'time'
require 'benchmark'

class Rane
  attr_accessor :image
  
  def initialize(i)
    @image = i
  end
  
  def blit_to(screen, x, y)
    screen.blit(x, y, self.image)
  end
  
end 


class Rayer
  attr_reader   :tilewide
  attr_reader   :tilehigh
  attr_reader   :layer
  attr_accessor :tileset
    
  def initialize(wide, high, tilewide, tilehigh)  
    @layer      = []
    @high       = high
    @wide       = wide
    for index in (0...@high)
      @layer[index] = []
      for jdex in (0...@wide)
        @layer[index].push(nil)
      end  
    end
    @tilehigh   = tilehigh
    @tilewide   = tilewide
    @realhigh   = @tilehigh * @high
    @realwide   = @tilewide * @wide
    @x          = 0
    @y          = 0
    @vx         = 0
    @vy         = 0
  end
  
  
  # Sets the tile in this layer to the tile with this tile index
  def set(x, y, slate)
    if (x < 0) or (y<0) or ( x >= @wide) or (y >= @high)
      raise "Index #{x} #{y} #{tile_id} not valid." 
    end  
    @layer[y][x]      = slate 
  end
  
  # Gets the tile of the tile at the given x,y coordinates,
  # or nil if out of bounds 
  def get(x, y) 
    gx = x; gy = y
    return nil if gy > @high or gx > @wide or gy < 0 or gx < 0  
    return @layer[gy][gx]
  end
  
  # Returns the actual tile at the given x and y coordinates, 
  # or nil if the coordinates are out of bounds
  def get_tile(x, y) 
    gx = x; gy = y
    return nil if gy > @high or gx > @wide or gy < 0 or gx < 0  
    return @layer[gy][gx]
  end
  

  def draw(screen, x, y)
    txstart     = ( x / @tilewide )
    tystart     = ( y / @tilehigh )
    xtilestop   = (screen.w / @tilewide) + 1
    ytilestop   = (screen.h / @tilehigh) + 1
    txstop      = xtilestop + txstart
    tystop      = ytilestop + tystart
    drawx       = 0
    drawy       = 0
    row         = nil
    aidimage    = nil
    nilcount    = 0
    return if (txstart >= @wide or tystart >= @high) 
    txstart = 0    if (txstart < 0) 
    tystart = 0    if (tystart < 0)
    txstop  = @wide if (txstop > @wide) 
    tystop  = @high if (tystop > @high) 
    drawy   = -y + ( (tystart-1) * @tilehigh )
    tydex   = tystart
    # for tydex in (tystart...tystop)
    while tydex < tystop do 
      drawy   += @tilehigh;
      drawx   = -x + ( (txstart-1) * @tilewide )
      row     = @layer[tydex]
      # for txdex in (txstart...txstop) do
      txdex = txstart
      while txdex < txstop
        drawx   += @tilewide        
        aidtile  = row[txdex]
        unless aidtile.nil? then
          aidtile.blit_to(screen, x, y)
          # screen.blitsheet(drawx, drawy, aidtile)
        end   
        txdex += 1
      end
      tydex  += 1
    end
  end


  def w
    return @realwide
  end

  def h
    return @realhigh
  end
  
  def tw
    return @wide
  end

  def th
    return @high
  end


end



def timer(*args)
  start = Time.now
  yield(*args)
  stop = Time.now
  return stop - start
end



game    = Gari::Game.new
full    = false
screen  = game.openscreen(640, 480, full)
yellow  = Gari::Color.rgb(255, 255, 0)
orange  = Gari::Color.rgb(255, 191, 0)
blue    = Gari::Color.rgb(0  , 0, 127)
green   = Gari::Color.rgb(0  , 127, 0)

im1     = Gari::Image.new(32,  32, 16, 0).optimize(0,0)
im1.slab(0,0,32,32, yellow)
im2     = Gari::Image.new(32,  32, 16, 0).optimize(0,0)
im2.slab(0,0,32,32, orange)

buf     = Gari::Image.new(640, 480, 16, 0).optimize(0,0)

sh1     = Gari::Sheet.new(im1)
sh2     = Gari::Sheet.new(im2)
ra1     = Rane.new(im1)
ra2     = Rane.new(im2)

layer   = Gari::Layer.new(20, 20, 32, 32)
rayer   = Rayer.new(20, 20, 32, 32)
(0..19).each { |i| (0..19).each { |j| layer.set(i, j, sh1) } }
(0..19).each { |i| (0..19).each { |j| rayer.set(i, j, ra1) } }

p layer.get(0,0)
layer.set(1,1, sh2)
p layer.get(1,1)
rayer.set(1,1, sh2)

p sh1.image
screen.blit(20, 20, im1)
im1     = nil
GC.start
p sh1.image
screen.blitsheet(50, 50, sh1)

t1 = timer do
  1.times do
    layer.draw(screen, 77 , 77)
  end 
end
  
t2 = timer do
  1.times do
    rayer.draw(screen, 77, 77)
  end 
end

p t1, t2

# The C function is of comparable speed as the Ruby one. Goes to show Ruby 
# 1.9.2 is much faster now than 1.8.x before, I guess.

game.update
sleep(0.5)
sh1.image = im2
# layer.draw(screen, 5 , 5)
game.update


go = true
oldx = 0
oldy = 0

while go
  ev = Gari::Event.poll 
  if ev
    case ev.kind
      when Gari::Event::QUIT
        go = false
      when Gari::Event::MOUSEMOVE
        screen.dot(ev.x, ev.y, blue);
        screen.line(oldx, oldy, ev.x-oldx, ev.y-oldy, blue)
        oldx = ev.x
        oldy = ev.y  
      else 
        # p ev.kind
    end   
  end    
  game.update
end












