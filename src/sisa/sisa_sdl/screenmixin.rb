
require 'delegate'



module Sisa_SDL
  module ScreenMixin
    module ClassDefs
      # Gets the currently active SDL video surface
      def screen
        vs      = SDL::Screen.get rescue nil
        return vs
      end  
      
      # Gets the pixel format of the currently active SDL video surface
      def screen_format
        vs      = SDL::Screen.get rescue nil
        return nil unless vs 
        return vs.format
      end  
    
      # Tries to make a screen in a video mode with the given resolution, fullscreen or not.
      # If the screen is already open, it returns that same screen.  
      # Display may have different depth than the one requested, and may not 
      # be fullscreen or double buffered, even if that was requested, in case such a screen is unavailable.
      # Returns nil on failiure. Retuns an SDL::Screen on success
      def make(wide = 640, high =480, fullscreen = false, depth = nil, doublebuf = true)
        vs         = screen
        return vs if vs
        return make!(wide, high, fullscreen, depth, doublebuf)
      end
    
      # Tries to make a video mode with the given resolution, fullscreen or not,
      # even if the screen is already opened.   
      # Display may have different depth than the one requested, and may not 
      # be fullscreen or double buffered, even if that was requested, in case such a screen is unavailable.
      # Returns nil on failiure. Retuns an SDL::Screen on success
      def make!(wide = 640, high =480, fullscreen = false, depth = nil, doublebuf = true)
        # First inittialize SDL graphics if needed
        self.init   
        # Get bbp from info, defaulting to
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
        return vs
      end
    
      
        
      # Calls SDL_init for the video if that wasn't already done before
      def init()
        unless (SDL.inited_system(SDL::INIT_VIDEO) & SDL::INIT_VIDEO) == SDL::INIT_VIDEO
          SDL.init(SDL::INIT_VIDEO) 
        end  
      end
      
      # Returns the currently active screen
      def active
        return self.screen  
      end
  
    end  # ClassDefs
    
    # Sets the caption (text on top of the window) of the main screen, 
    # and the icon, if possible. May fail silently.
    def caption=(title, icon = nil)
      @caption = title 
      @icon   = icon     
      SDL::WM.set_caption(@caption, @icon) rescue nil 
      # We don't care too much if this fails.
    end
    
  end # module ScreenMixin  
end # module Sisa



