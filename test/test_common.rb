$: << '../src'

require 'minitest/autorun'
require 'sdl'
require 'sisa'
require 'rogaru'

module Test
  class Common < MiniTest::Unit::TestCase
  # Utility functions for common use in tests.
  
    def open_screen(wide = 640, high = 480, fullscreen  = false)
      @screen         = Sisa::Screen.make(wide, high, fullscreen)
      @screen.caption  = 'Sisa Test'
      return @screen
    end
    
    # Transparent black is the forbidden color
    FORBIDDEN_COLOR = Sisa.color(0,0,0,255)
    
    # Wipes the screen to the forbidden color
    def wipe_screen
      @screen.quickfill_rectangle(0, 0, @screen.w, @screen.h, FORBIDDEN_COLOR)
    end
    
    # Asserts that the point at x, y has been drawn and is not 
    # transparent black or the given color    
    def assert_drawn(x, y, forbidden = FORBIDDEN_COLOR)
       color = @screen.get_point(x,y)
       assert color != forbidden , "Not drawn correctly at #{x} #{y}"
    end
    
    # Asserts that the point at x, y has not been drawn to and is not 
    # transparent black or the given color    
    def assert_not_drawn(x, y, forbidden = FORBIDDEN_COLOR)
       color = @screen.get_point(x,y)
       assert color == forbidden , "Drawn unneededly at #{x} #{y}"
    end
    
    # Default setup opens the screen in @screen
    def setup
      open_screen
    end
    
    # Default teardown stops SDL
    def teardown
      SDL.quit()
    end
    
    # Just to ensure the screen open is tested
    def test_common
      assert @screen, "Could not open screen" 
    end
    
  end  
end  
