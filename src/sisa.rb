# A Simple API for Ruby/SDL. 
# This is a simplified API for Ruby/SDL,
# Partially based on mixins. 

require 'sdl'

# Some helper classes and modules that are not in Ruby/SDL
module Sisa
  autoload :Color       , 'sisa/color.rb'
  autoload :Font        , 'sisa/font.rb'
  autoload :SurfaceMixin, 'sisa/surfacemixin.rb'
  autoload :ScreenMixin , 'sisa/screenmixin.rb'
end

# Extend SDL module with mixins.
module SDL
  class Surface
    include Sisa::SurfaceMixin
    extend  Sisa::SurfaceMixin::ClassDefs
  end
  class Screen
    include Sisa::ScreenMixin
    extend  Sisa::ScreenMixin::ClassDefs
  end
end    

module Sisa   
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
  
  
  def self.load_alpha(filename)
    return SDL::Surface.load_alpha(filename)
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
  
  sleep 3


end
