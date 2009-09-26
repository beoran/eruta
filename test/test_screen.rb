# Tests screen related functions
require 'test_common'
require 'sdl'

END { SDL.quit() }

module Test
  class Screen < Test::Common
  
    def test_screen
      wide        = 640 
      high        = 480 
      fullscreen  = true
      depth       = nil
      doublebuf   = false
      hardware    = true
      glblit      = false
      SDL.init(SDL::INIT_AUDIO | SDL::INIT_VIDEO)
      # SDL::Screen.init
      modes = SDL::Screen.list_modes(SDL::FULLSCREEN|SDL::HWSURFACE)
      p modes
      check = SDL::Screen.check_mode(wide,high,16,SDL::FULLSCREEN|SDL::HWSURFACE) 
      p check
      # p SDL::Screen.info
      bpp        = SDL::Screen.info.bpp rescue 32   
      depth    ||= bpp
      flags      = SDL::ANYFORMAT
      flags     |= SDL::HWSURFACE  if hardware
      flags     |= SDL::DOUBLEBUF  if doublebuf
      flags     |= SDL::FULLSCREEN | SDL::HWSURFACE if fullscreen
      flags     |= SDL::OPENGL | SDL::OPENGLBLIT if glblit
      new_depth  = SDL::Screen.check_mode(wide, high, depth, flags)
      puts "Screen depth OK? #{new_depth.inspect}"
      # Try again with minimal requirements.
      if new_depth == 0
        flags      = SDL::ANYFORMAT
        new_depth  = SDL::Screen.check_mode(wide, high, depth, flags)
      end
      puts "Screen depth OK? #{new_depth.inspect}"
      return nil unless new_depth != 0
      screen       = SDL::Screen.open(wide, high, new_depth, flags) rescue nil     
      assert screen 
      assert_equal screen.w , 640
      assert_equal screen.h , 480
      surf  = SDL::Surface.load('test_tile.png')
      surf  = surf.display_format_alpha
      start = Time.now
      repeat= 200
      xx    = 0
      yy    = 0
      repeat.times do |i|
        while xx < 640
          xx += 32
          while yy < 480
            yy += 32
            screen.put(surf, xx, yy)
          end  
          yy = 0
        end
        xx   = 0
        screen.flip
      end
      stop = Time.now 
      diff = stop - start
      puts "Frames #{repeat}, #{diff}, fps : #{repeat.to_f/diff.to_f}"
      SDL.quit()
    end
    
  
  end
end  
