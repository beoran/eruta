# Sisa::Mixin::Surface is a mixin for SDL::Surface 
# that helps making it easier to use, and more Rubyish.
# For example, the drawing methods'colors are mapped automagically, 
# and are taken from an array-like object [r, g, b, a].

module Sisa_SDL  
  module SurfaceMixin 
    # Class functions
    module ClassDefs
      
      # Returns the masks for this bpp without alpha
      def masks_for_depth_noalpha(bpp)
        amask = 0       
        case bpp
          when 8
            rmask = 0xFF >> 6 << 5
            gmask = 0xFF >> 5 << 2
            bmask = 0xFF >> 6
          when 12
            rmask = 0xFF >> 4 << 8
            gmask = 0xFF >> 4 << 4
            bmask = 0xFF >> 4
          when 15
            rmask = 0xFF >> 3 << 10
            gmask = 0xFF >> 3 << 5
            bmask = 0xFF >> 3
          when 16
            rmask = 0xFF >> 3 << 11
            gmask = 0xFF >> 2 << 5
            bmask = 0xFF >> 3
          when 24, 32
            rmask = 0xFF << 16
            gmask = 0xFF << 8
            bmask = 0xFF
          else
            raise("Cannot create masks for this color depth and alpha settings")
        end   
        return rmask, gmask, bmask, amask, bpp  
      end
      
      #  Returns the masks for this bpp with alpha
      def masks_for_depth_alpha(bpp)
        case bpp      
          when 16 
            rmask = 0xF << 8
            gmask = 0xF << 4
            bmask = 0xF
            amask = 0xF << 12
            # 4444 format
          when 32 
            rmask = 0xFF << 16
            gmask = 0xFF << 8
            bmask = 0xFF
            amask = 0xFF << 24
            # 8888 format
          else 
            raise("Cannot create masks for this colod depth and alpha settings")
        end
        return rmask, gmask, bmask, amask, bpp  
      end  
  
      # Get the masks from the screen, adjusting for alpha if needed
      def masks_from_screen(alpha)    
        videosurf = SDL::Screen.get
        unless videosurf
          raise "Open screen before creating images without specifying bpp."
        end
        
        if alpha
          format  = videosurf.display_format_alpha.format
          # Convert the video surface to alpha and get that format. 
          # XXX That this is likely to be very slow and wasteful of memory, so, 
          # is there a different way to do it???
        else  
          format  = videosurf.format
        end  
        rmask     = format.Rmask
        gmask     = format.Gmask
        bmask     = format.Bmask
        amask     = format.Amask
        return rmask, gmask, bmask, amask, format.bpp  
      end  
      
      # Returns the masks for this alpha and bpp, and also the new bpp to use 
      def masks_for_depth(bpp, alpha)
        unless bpp    # Determine from screen if no bpp is given.
          return masks_from_screen(alpha)           
        end
        if alpha      # Otherwise calculate manually
          return masks_for_depth_alpha(bpp)
        else  
          return masks_for_depth_noalpha(bpp)        
        end
      end
    
      # Creates a new bitmap with the give width, height, and bits per pixel 
      # (color depth). You can also set the color key, whether or not to use 
      # alpha (transparency), and whether rle encoding should be used, and if
      # SDL should try to create the surface in hardware.
      # You must first open the screen before using this method, as it sets the 
      # parameters of this image from the screen's format. 
      def make(wide        , high      , bpp = 32    , colorkey = nil, 
               alpha = true, rle = true, hard = false)    
        flags     = ( hard ? SDL::HWSURFACE : SDL::SWSURFACE)
        flags     = flags & SDL::SRCCOLORKEY if colorkey 
        flags     = flags & SDL::SRCALPHA    if alpha
        flags     = flags & SDL::RLEACCEL    if rle
        # Flags from parameters      
        rmask, gmask, bmask, amask, newbpp  = masks_for_depth(bpp, alpha)
        # Determine the masks to use, and the new bpp if any    
        handle    = SDL::Surface.new(flags, wide, high, newbpp, 
                                     rmask, gmask, bmask, amask)    
        # Get handle
        unless handle 
          raise "Failed to create image!"
        end
        # Set colorkey if needed
        if colorkey and colorkey.respond_to? :to_a
          handle.set_color_key(SDL::SRCCOLORKEY, format.map_rgb(*colorkey))
        end
        # set alpha if needed
        if alpha and alpha.respond_to? :to_f
          handle.set_alpha(SDL::SRCALPHA, alpha)
        end
        return handle
      end
    
      # Loads this image from a filename. 
      def load_filename(filename) 
        surface = SDL::Surface.load(filename)
        return surface
      end 
      
      # Loads an solid image, prepares it for display.
      def load_solid(filename)
        image = load_filename(filename)
        return image.to_display()
      end	
      
      # Loads an image with a colorkey, prepares it for display.
      # By default, the colorkey is bright pink (#FF00FF), like in the Allegro game library
      def load_colorkey(filename, colorkey = Sisa::SurfaceMixin::DEFAULT_COLORKEY)
        image = load_filename(filename)
        image.set_colorkey(colorkey)
        return image.to_display_alpha()
      end	
      
      # Loads an image with alpha, prepares it for display.
      def load_alpha(filename)
        surface = load_filename(filename)
        return surface.to_display_alpha()
      end	
  
    end  # ClassMethods
    
    # Initialize the Sisa surface
    def initialize(*args)
      super(*args)
      @use_rle        = true
      @rotozoom_flags = SDL::TRANSFORM_TMAP
    end
  
    DEFAULT_COLORKEY = [ 255, 0, 255 ] 
    # Default color key is bright pink, like in Allegro
    
    # Whether or not to use rle compression. True by default.
    attr_accessor :use_rle
    
    # Convert this image to one which is compatible with the screen, making it faster to blit
    def to_display()
      self.display_format()
    end 
    
    # Convert this image to one which is compatible with the screen, making it faster to blit.
    # Use this if the surface has alpha values.  
    def to_display_alpha()
      self.display_format_alpha()
    end 
    
    # Set the color key of this image      
    def colorkey=(color)
      unless color
        self.set_color_key(0, color) # disable color key
      end
      @use_rle ||= true   
      key        = self.map_rgb(*color) # To internal format       
      flag       = SDL::SRCCOLORKEY
      flag       = flag & SDL::RLEACCEL if @use_rle    
      self.set_color_key(flag, key)    
      # Set flags and set color key
      return color
    end    
    
    # Sets the per surface alpha of this image
    def alpha=(alpha)
      unless alpha
        self.set_alpha(0, alpha) # disable alpha
      end
      @use_rle  ||= true
      alpha       = alpha.to_i # Alpha to integer    
      flag        = SDL::SRCALPHA
      flag        = flag & SDL::RLEACCEL if @use_rle    
      self.set_alpha(flag, key)    
      # Set flags
      return color
    end    
      
    # Retuns the low level pixel format of this drawable.
    def pixel_format()
      return self.format
    end
      
    # Maps a color to the internal color value used by the low level system
    def map_color(color)
      r, g, b, a   = *(color.to_a)
      if a
        internal   = self.pixel_format.map_rgba(r, g, b, a)
      else   
        internal   = self.pixel_format.map_rgb(r, g, b)
      end  
      return internal
    end
    
    # Draws straight line fromx1, y1 to x1+ww, y1+hh 
    def put_line(x1, y1, ww, hh, color, aa = true, alpha = nil) 
      ci = map_color(color)
      self.draw_line(x1, y1, x1 + ww, y1 + hh, ci, aa, alpha)
      return self
    end
    
    # Draws empty rectangle.
    def put_rectangle(x1, y1, ww, hh, color, alpha = nil) 
      ci  = map_color(color)
      self.draw_rect(x1, y1, ww, hh, ci, false, alpha)
      return self
    end
    
    # Draws a filled rectangle
    def fill_rectangle(x1, y1, ww, hh, color, alpha = nil) 
      ci = map_color(color)
      self.draw_rect(x1, y1, ww, hh, ci, true, alpha)
      return self
    end
    
    # Draws an empty circle with top left corner x1, y1, and with given diameter
    def put_circle(x1, y1, diameter, color, alpha = nil)     
      ci    = map_color(color)
      radius= diameter / 2
      cx    = x1       + radius
      cy    = y1       + radius 
      self.draw_circle(cx, cy, radius , ci, false, alpha)
      return self
    end

    # Draws an filled circle with top left corner at x1, y1, and with given diameter
    def fill_circle(x1, y1, diameter, color, alpha = nil) 
      ci    = map_color(color)
      radius= diameter / 2
      cx    = x1       + radius
      cy    = y1       + radius 
      self.draw_circle(cx, cy, radius, ci, true, alpha)
      return self
    end

    # Draws an empty ellipse
    def put_ellipse(x1, y1, rx, ry, color, alpha = nil) 
      ci = map_color(color)
      self.draw_ellipse(x1, y1, rx, ry , ci, false, alpha)
      return self
    end

    # Draws a filled ellipse
    def fill_ellipse(x1, y1, rx, ry, color, alpha = nil) 
      ci = map_color(color)
      self.draw_ellipse(x1, y1, rx, ry , ci, true, alpha)
      return self
    end
    
    # Draws a bezier curve from x1,y1 to x2,y2, using the 2 control points
    def put_curve(x1, y1, x2, y2, cx1, cy1, cx2, cy2, color, alpha = nil)
      ci    = map_color(color)
      level = 4.0
      aa    = false
      self.draw_bezier(x1, y1, x2, y2, cx1, cx2, cy1, cy2, level = 4.0, ci, aa, alpha)
    end 
      
    # Quickly fills a rectangle on this drawable with a given color. Does no blending. 
    def quickfill_rectangle(x1, y1, ww, hh, color)  
      ci = map_color(color)
      self.fill_rect(x1, y1, ww, hh, ci)
      return self
    end
    
    # Fills the whole drawable with the given color. Does no blending.
    def fill(color) 
      ci = map_color(color)
      quickfill_rectangle(0, 0, self.wide, self.high, color)
      return self
    end
    
    # Draws a single pixel with the given color.
    def put_point(x, y, color)
      ci = map_color(color)
      put_pixel(x, y, ci)
    end
    
    alias :line       :put_line
    alias :circle     :put_circle
    alias :rectangle  :put_rectangle
    alias :ellipse    :put_ellipse
            
    # Aliases for the size of this drawable    
    def width 
      return self.w
    end
      
    def height
      return self.h
    end
    
    alias_method :wide  , :width
    alias_method :high  , :height
    
    # Blitting
    # Puts a bitmap to self at the given coordinates 
    def put_bitmap(xx, yy, bitmap)
      SDL::Surface.blit(bitmap, 0, 0, 0, 0, self, xx, yy)
      return self
    end
    
    
    # Blit the whole of this image to the target at the given 
    # x and y coordinates in the target
    def blit(target, xx, yy)
      SDL::Surface.blit(self, 0, 0, 0, 0, target, xx, yy)
      return self
    end
    
    # Blit the a rectangular part of this image to the target at the 
    # given x and y coordinates in this target
    def blit_rectangle(target, xx, yy, srcx, srcy, srcw, srch)
      SDL::Surface.blit(self, srcx, srcy, srcw, srch, target, xx, yy)
      return self
    end
    
    # Returns a new bitmap with a copy of the image inside the 
    # rectangle at x,y with dimensions w, h 
    def copy_rectangle(x, y, w, h)
      return self.copy_rect(x, y, w, h)
    end    
    
    # Transforms self and blits it to the target bitmap
    def rotozoom_blit(target, xx, yy, xscale = 1, yscale = 1, angle = 0, cx = nil, cy = nil)
      cx              ||= 0
      cy              ||= 0
      @rotozoom_flags ||= 0 # SDL::Surface::TRANSFORM_TMAP # , 0 # SDL::TRANSFORM_TMAP
      SDL::Surface.transform_blit(self, target, angle, xscale, yscale, cx, cy, xx, yy, @rotozoom_flags)
    end 
    
    # Draws a scaled fragment rectangle from self (rectangle starts at 
    # copy_x, copy_y and is  copy_w by copy_h in size )    
    # and blits its scaled to target at xx and yy
    def scale_blit(target, xx, yy, copy_x, copy_y, copy_w, copy_h, new_w, new_h) 
      # Need to do to_display alpha to be sure alpha in self is correctly 
      # blitted as transparent
      mrect  = self.to_display_alpha.copy_rect(copy_x, copy_y, copy_w, copy_h)
      # Need a fudge factor here because transform_blit still makes 
      # black lines appear for large images. And transform_draw doesn't work 
      # either, because it doesn't clip to the screen edges correctly.
      ffx    = (new_w > 180 ? 1 : 0)
      ffy    = (new_h > 180 ? 1 : 0)        
      xscale = (new_w.to_f + ffx) / mrect.w.to_f
      yscale = (new_h.to_f + ffy) / mrect.h.to_f
      # Set scale to 1 if close to 1
      # xscale = 1 if (xscale - 1.0).abs < 0.1
      # yscale = 1 if (yscale - 1.0).abs < 0.1
      # Transform_blit is messy and creates black pixels at it's edges...
      # Need to set this clip rectangles as otherwise transform_blit overflows
      # and creates a black edge.
      mrect.set_clip_rect(0, 0, copy_w, copy_h)
      target.set_clip_rect(xx, yy, new_w, new_h) 
      SDL::Surface.transform_blit(mrect, target, 0, xscale, yscale, 
      0, 0, xx, yy, SDL::Surface::TRANSFORM_SAFE)
      # Use SDL::Surface::TRANSFORM_TMAP somehow???
      target.set_clip_rect(0, 0, target.w, target.h) 
      mrect.destroy
    end
     
    # Blits to target a according to a 9 part scaling alorithm. 
    # This splits the bitmap in 9 parts, 
    # keeping the 4 corners unscaled, but scaling the 4 edges and the center  
    # according to the desired size. The 4 corners should be rectangles of
    # identical size corner_w by corner_h in the original image.
    # new_w and new_h are the new dimensions the new image should have. 
    # This is useful for GUI backgrounds.
    def scale_9(target, xx, yy, new_w, new_h, corner_w = 16, corner_h = nil)
      corner_h  ||= corner_w      
      center_old_w= self.w - (corner_w * 2) 
      center_old_h= self.h - (corner_h * 2)
      center_new_w= new_w  - (corner_w * 2) 
      center_new_h= new_h  - (corner_h * 2) 
      # First blit the four border strips  
      # Top strip
      self.scale_blit(target, xx + corner_w, yy      , corner_w    , 0, 
                      center_old_w, corner_h, center_new_w , corner_h)
      # Bottom strip
      self.scale_blit(target  , xx + corner_w, yy + corner_h + center_new_h,
                      corner_w, corner_h + center_old_h, 
                      center_old_w, corner_h, center_new_w, corner_h)
      # Left strip
       self.scale_blit(target  , xx , yy + corner_h,
                      0, corner_h, corner_w, center_old_h,  
                      corner_w, center_new_h)

      # Right strip
       self.scale_blit(target  , xx + corner_w + center_new_w, yy + corner_h,
                       corner_w + center_old_w, corner_h, corner_w, center_old_h,  corner_w, center_new_h)                       
  
      # Now, blit the four corners
            
      # Top left to top left
      srcy        = srcx   = 0
      tary        = yy
      tarx        = xx
      self.blit_rectangle(target, tarx, tary, srcx, srcy, corner_w, corner_h)
      # Top right to top right
      srcx        = self.w - corner_w 
      tarx        = xx + new_w  - corner_w      
      self.blit_rectangle(target, tarx, tary, srcx, srcy, corner_w, corner_h)
      # Bottom left to bottom left
      srcx        = 0      
      srcy        = self.h - corner_h
      tarx        = xx
      tary        = yy + new_h  - corner_h       
      self.blit_rectangle(target, tarx, tary, srcx, srcy, corner_w, corner_h)
      # Bottom right to bottom right
      srcx        = self.w - corner_w
      tarx        = xx + new_w  - corner_w
      self.blit_rectangle(target, tarx, tary, srcx, srcy, corner_w, corner_h)
      
    # Next, blit the middle rectangle of self scaled to the new dimensions
    # XXX? Must it be made a bit bigger ythat it should for library reasons?
      self.scale_blit(target, xx + corner_w , yy + corner_h ,
                      corner_w, corner_h, center_old_w, center_old_h, 
                      center_new_w, center_new_h)
                

   end      
       
    
    
    # Gets the R, G, B and A values of the point at the given coordinates,
    # or nil if out of bounds
    def get_point(x, y)
      pixel   = self.get_pixel(x, y) rescue nil
      return nil unless pixel
      pform   = self.pixel_format
      return Sisa::Color.new(*pform.get_rgba(pixel))
    end
    
  end # module SurfaceMixin  
end # module Sisa 
