
#
# Roleplaying Game engine for Rubygames
#
require 'rubygems'
require 'sdl'


module Rogaru
  autoload :Fairy   , 'rogaru/fairy'
  autoload :Image   , 'rogaru/image'
  autoload :Screen  , 'rogaru/screen'
  autoload :Space   , 'rogaru/space'
  autoload :Tilemap , 'rogaru/tilemap'
  autoload :UI      , 'rogaru/ui'
  autoload :Utility , 'rogaru/utility'
  autoload :Walker  , 'rogaru/walker'
  
  
  # Class that models an FPS timer or clock 
  # Todo: put in it's own file
  class Clock
    require 'time'
    
    attr_reader :frames
    
    # Creates a new clock and starts it's timer.  
    def initialize()
      @start  = SDL.get_ticks
      @frames = 0
      @now    = @start
    end

    # Call this whenever you drew a frame.
    def tick
      @frames += 1 
    end
    
    # Returns how many milliseconds have passed since starting this clock.
    def elapsed
      @now    = SDL.get_ticks 
    end

    # Determines the current framerate based on the ticks and time passed
    def framerate
      delta   = (elapsed - @start) / 1000.0 # Delta in seconds
      return 1 if delta  < 1.0
      return 1 if @frames < 1.0
      return  @frames.to_f / delta.to_f 
    end
    
  end



end
