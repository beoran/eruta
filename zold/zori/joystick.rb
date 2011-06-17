module Zori
  # Class that models the Joystick state
  class Joystick
  
    attr_accessor :x
    attr_accessor :y
    attr_accessor :active
    
    def make_joystick_cursor
      return Hanao.load_image('ui', 'cursor', 'joystick_0.png')
    end
  
    def initialize
      @buttons = {}
      @axis    = {}
      @hats    = {}
      @balls   = {}
      @cursor  = make_joystick_cursor
      @x       = 100
      @y       = 100
      @active  = true
    end
    
    def draw(screen)
      screen.blit(@x, @y, @cursor)
    end
    
  end
end    