# Copyright Bjorn De Meyer 2007. Can be used freely under the zlib license.

module Rogaru
  module Image
    
    def self.new_alpha(wide, high)
      aid = Surface.new(SDL::SRCALPHA, wide, high, 32, 0xff00000, 0x00ff0000, 0x0000ff00, 0x000000ff)
      return aid.display_format_alpha
    end

    def self.load_file(filename) 
      begin
        surface = SDL::Surface.load(filename)
        return surface
      rescue SDL::Error
        raise "Could not load image #{filename}"
      end	
    end 

    # Loads an solid image, prepares it for display.
    # By default, the colorkey is bright pink (#FF00FF), like in the Allegro game library
    def self.load_solid(filename)
      surface = load_file(filename)
      return surface.display_format()
    end	

    # Loads an image with a colorkey, prepares it for display.
    # By default, the colorkey is bright pink (#FF00FF), like in the Allegro game library
    def self.load_colorkey(filename, colorkey = [255,0,255])
      surface = load_file(filename)
      surface.set_color_key(SDL::SRCCOLORKEY, colorkey)
      return surface.display_format_alpha()
    end	

    # Loads an image with alpha, prepares it for display.
    def self.load_alpha(filename)
      surface = load_file(filename)
      return surface.display_format_alpha()
    end	


  end

end # module Image
