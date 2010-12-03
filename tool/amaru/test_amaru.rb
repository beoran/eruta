require 'sdl'
require 'amaru'

END { SDL.quit }

module Amaru
  class Layer 
    def to_s
      "<Amaru::Layer w #{w} h #{h} tile_w #{tile_w} tile_h #{tile_h}"
    end
  end
end    

SDL.init(SDL::INIT_EVERYTHING)
flags      = SDL::FULLSCREEN
wide       = 640
high       = 480
depth      = 32
new_depth  = SDL::Screen.check_mode(wide, high, depth, flags)
screen     = SDL::Screen.open(wide, high, new_depth, flags)
tile       = SDL::Surface.load('test_tile.png');
tilea      = tile.display_format_alpha
tile2      = SDL::Surface.load('test_tile.png');
tilea2     = tile2.display_format_alpha 
 
# Convert for speed up.
layer      = Amaru::Layer.new(100, 100, 32, 32)
for x in (0...100) do 
  for y in (0...100) do 
    layer.set(x, y, tilea)  if y < 1
    layer.set(x, y, tilea2) if y > 0
  end
end
p tile.w 
p tile.h
p layer.get(2, 10)
tile = nil
GC.stress = true 
GC.start
puts layer.to_s
green = screen.map_rgb(0,200,0) 
# screen.set_clip_rect(0,0,100,100)
    
repeat_start = 1000        
repeat       = repeat_start 
start        = Time.now   
while repeat > 0 do
  # screen.fill_rect(0, 0, wide, high, green)
  layer.draw(screen, 0, 0, wide, high)
  screen.flip  
  # SDL::Screen#update_rect(x,y,w,h)
  repeat -= 1
end
stop = Time.now
delta = stop - start
puts "#{delta} #{repeat_start} -> #{repeat_start/delta}"
 


 
 
 
