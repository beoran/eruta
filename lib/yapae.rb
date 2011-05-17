#
# Yapae -- yet another pixel art editor. 
# Uses chunkypng for IO.
#
if $0 == __FILE__
  $: << '.'
end
require 'gari'
require 'zori'
require 'fimyfi'
require 'chunky_png'

class Yapae
  # initializes the application
  def initialize
    @game   = Gari::Game.new 
    @screen = @game.openscreen(1024, 768, false)
    @queue  = Gari::Event
    @ui     = Zori.open(@game, @queue)
    @image  = ChunkyPNG::Image.new(640,480);
    @colors = {}
    try_image
  end
  
  def hex_color(hex)
    aid = @colors[hex]
    return aid if aid
    @colors[hex] = ChunkyPNG::Color.from_hex(hex)
    return @colors[hex] 
  end
  
  def image_putpixel(x, y, hex)
    col           = self.hex_color(hex)
    @image[x, y]  = col
  end  
  
  def image_slab(x, y, w, h, hex)
    @image.rect(x, y, x+w, y+h, self.hex_color(hex), self.hex_color(hex))
  end
  
  def try_image
    self.image_slab(0, 0, 32, 32, "0x778899")
    self.image_slab(32, 32, 32, 32, "0xffff00")
    @image.save("try.png", :color_mode => ChunkyPNG::COLOR_INDEXED)
    @image2 = ChunkyPNG::Image.from_file('../data/image/tile/tiles_1000_world.png')
    p @image2.palette
    @image2.save("try2.png", :color_mode => ChunkyPNG::COLOR_INDEXED)
  end
  
  
  # Renders the application and updates the app's state
  def update
    @ui.update
    @ui.draw
    @game.update
  end
  
  # Runs the application.
  def main
    until @ui.done do
      self.update
    end
  end
  

  # Instantiates the application and starts it.  
  def self.main
    @app = self.new
    @app.main
  end

end

if $0 == __FILE__
  Yapae.main
end
