#
# Copyright Bjorn De Meyer 2007. Can be used freely under the zlib license.
# Various utility  functions
#

module Rogaru
  module Utility
  def self.used_memory()
    size = 0 
    count = 0 
    # GC.start
     ObjectSpace.each_object do |o| 
       count+=1 
     end     
    return [count, -1] 
  end

  def self.screen_make(wide, high, fullscreen, title = "", bpp = 16, doublebuf = true) 
    return Sisa::Screen.make!(wide, high, fullscreen)
  end 
  
  def self.event_queue
    return @queue
  end
    
  def self.event_setup(nomouse = false)
    raise "obsolete"
  end

  def self.joystick_setup()
    SDL.init(SDL::INIT_JOYSTICK)
    joysticks = []
    stop      = SDL::Joystick.num - 1
    for joynum in 0..stop do
      puts "Setting up joystick: " + SDL::Joystick.index_name( joynum )
      joysticks[joynum] = SDL::Joystick.open(joynum)
    end
    return joysticks
  end
  end
end


