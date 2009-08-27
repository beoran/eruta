# Tests screen related functions
require 'test_common'
require 'sdl'

module Test
  class Screen < Test::Common
  
    def test_screen
      wide        = 640 
      high        = 480 
      fullscreen  = false
      depth       = nil
      doublebuf   = true 
      SDL::Screen.init
      bpp        = SDL::Screen.info.bpp rescue 32   
      depth    ||= bpp
      flags      = SDL::HWSURFACE | SDL::ANYFORMAT
      flags     |= SDL::DOUBLEBUF  if doublebuf
      flags     |= SDL::FULLSCREEN if fullscreen
      new_depth  = SDL::Screen.check_mode(wide, high, depth, flags)
      # Try again with minimal requirements.
      if new_depth == 0
        flags      = SDL::HWSURFACE | SDL::ANYFORMAT
        new_depth  = SDL::Screen.check_mode(wide, high, depth, flags)
      end
      return nil unless new_depth != 0
      vs           = SDL::Screen.open(wide, high, new_depth, flags) rescue nil
      assert vs 
      assert_equal vs.w , 640
      assert_equal vs.h , 480
    end
    
  
  end
end  
