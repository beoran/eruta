#
# Rubygame driver for Sisa
#
# Some helper classes and modules that are not in Ruby/SDL
require 'sdl'

module Sisa_SDL
  autoload :Color       , 'sisa/sisa_sdl/color.rb'
  autoload :Font        , 'sisa/sisa_sdl/font.rb'
  autoload :SurfaceMixin, 'sisa/sisa_sdl/surfacemixin.rb'
  autoload :ScreenMixin , 'sisa/sisa_sdl/screenmixin.rb'
end

# Extend SDL module with mixins.
module SDL
  class Surface
    include Sisa_SDL::SurfaceMixin
    extend  Sisa_SDL::SurfaceMixin::ClassDefs
  end
  class Screen
    include Sisa_SDL::ScreenMixin
    extend  Sisa_SDL::ScreenMixin::ClassDefs
  end
end    

module Sisa_SDL   
  Key       = SDL::Key  
  Event     = SDL::Event
  Screen    = SDL::Screen
  Surface   = SDL::Surface
  Rect      = SDL::Rect
  Joystick  = SDL::Joystick
  Mouse     = SDL::Mouse
    
  
  class Event
    JoyDown = SDL::Event::JoyButtonDown
    JoyUp   = SDL::Event::JoyButtonUp
    
    # Returns a hash of the names of and the classes of the events  
    # Sisa::Event.queue may emit
    def self.list()
      result = {}
      for name in SDL::Event.constants do
        klass             = SDL::Event.const_get(name);
        result[name]      = klass
      end
      return result
    end    
    
    # Allows to call Sisa::Event.each to poll all pending events
    def self.each
      event = self.poll
      while event  
        yield event
        event = self.poll
      end        
    end
    
    # Returns an event queue that can be polled with #poll, and to which
    # synthetic events can be added with #push
    def self.queue()
      # Enable joystick polling always 
      SDL::Joystick.poll= true
      # Enable unicode always.      
      SDL::Event.enable_unicode unless SDL::Event.enable_unicode?
      return SDL::Event
    end  
  end
  

  
  class Joystick
  
    def self.active?
      return self.joysticks.size > 0
    end
  
    def self.joysticks
      return @joysticks ||= []
    end
    
    def self.open_all
      @joysticks = [] 
      Sisa.init_once(SDL::INIT_JOYSTICK)
      num_joy = SDL::Joystick.num
      printf("Found %d joystick:\n", num_joy)
      num_joy.times do |i|
        puts SDL::Joystick.index_name(i)
        js = SDL::Joystick.open(i) unless SDL::Joystick.open?(i)
        @joysticks[i] = js if js 
      end
      at_exit do # Ensure joystick cleanup
        @joysticks.each { |j| j.close } 
      end      
    end  
  end
  
  def self.init?(sys)
    return SDL.inited_system(sys) != 0
  end
  
  def self.init_once(sys)
    SDL.init(sys) unless self.init?(sys)
  end
  
  def self.init_all()
    SDL.init(SDL::INIT_EVERYTHING) 
    at_exit do
      SDL.quit
    end  
  end
  
  
  def self.load_alpha(filename)
    return Sisa::Surface.load_alpha(filename)
  end
  
  def self.load_solid(filename)
    return Sisa::Surface.load_solid(filename)
  end

  
end



#   class SisaSDL
#     autoload :Drawing , 'sisa/sdl_diver/drawing'
#     autoload :Event   , 'sisa/sdl_diver/event'
#     autoload :Font    , 'sisa/sdl_diver/font'
#     autoload :Joystick, 'sisa/sdl_diver/joystick'
#     autoload :Key     , 'sisa/sdl_diver/key'
#     autoload :Mouse   , 'sisa/sdl_diver/mouse'
#     autoload :Music   , 'sisa/sdl_diver/music'
#     autoload :Rect    , 'sisa/sdl_diver/rect'
#     autoload :Screen  , 'sisa/sdl_diver/screen'
#     autoload :Surface , 'sisa/sdl_diver/surface'
#     autoload :Sound   , 'sisa/sdl_diver/sound'
#     
# #     Key       = SDL::Key  
# #     Event     = SDL::Event
# #     Screen    = SDL::Screen
# #     Surface   = SDL::Surface
# #     Rect      = SDL::Rect
# #     Joystick  = SDL::Joystick
# #     Mouse     = SDL::Mouse
#     
#     
#   end
# end  


