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
      doublebuf   = false 
      hardware    = false
      SDL::Screen.init
      bpp        = SDL::Screen.info.bpp rescue 32   
      depth    ||= bpp
      flags      = SDL::ANYFORMAT
      flags     |= SDL::HWSURFACE  if hardware
      flags     |= SDL::DOUBLEBUF  if doublebuf
      flags     |= SDL::FULLSCREEN | SDL::HWSURFACE if fullscreen
      new_depth  = SDL::Screen.check_mode(wide, high, depth, flags)
      puts "Screen depth OK? #{new_depth.inspect}"
      # Try again with minimal requirements.
      if new_depth == 0
        flags      = SDL::ANYFORMAT
        new_depth  = SDL::Screen.check_mode(wide, high, depth, flags)
      end
      puts "Screen depth OK? #{new_depth.inspect}"
      return nil unless new_depth != 0
      vs           = SDL::Screen.open(wide, high, new_depth, flags) rescue nil
      SDL.quit()
      assert vs 
      assert_equal vs.w , 640
      assert_equal vs.h , 480      
    end
    
  
  end
end  
