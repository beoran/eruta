# A Simple API for Ruby/SDL. 
# This is a simplified API for Ruby/SDL,
# Partially based on mixins. 


# Load the SISA that we need
SISA_USE_SDL = true
if SISA_USE_SDL
  require 'sdl'
  require 'sisa/sisa_sdl'
  Sisa = Sisa_SDL
elsif SISA_USE_RUBYGAME
  require 'rubygame'
  require 'sisa/sisa_rg'
  Sisa = Sisa_RG
elsif SISA_USE_GOSU
  require 'gosu'
  require 'sisa/sisa_gosu'
  Sisa = Sisa_GOSU
end

# Now extend sisa with portable functions
module Sisa  
# Creates a new Sisa::Color instance
  def self.color(*args)
    Sisa::Color.new(*args)
  end
end  



if $0 == __FILE__
  screen          = Sisa::Screen.make(640,480, false)
  screen.caption  = 'Sisa Test'
  try             = Sisa::Surface.make(32, 32, 32) 
  try.fill([255,255,0])
  spri            = Sisa::Surface.load_alpha('../share/image/sprite_test_2.png')
  try.blit(screen , 255, 255)
  spri.blit(screen, 100, 100)
  screen.flip
  p screen.wide
  p screen.high
  p screen.methods.sort - Object.methods
  sleep 3


end
