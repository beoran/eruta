# 
# Extendsions to the Gari C library in Ruby, for ease of implementation. 
# These may be moved down to C later if needed. 
# 

module Gari 
  class Image
    # Dot? checks whether the dot at the location x, y is of color color
    # Very useful as a unit test helper to check drawing functions.
    def dot?(x, y, col)
      return self.getdot(x, y) == col
    end
    
    # Returns half of the width (rounded down). Useful for centering, etc
    def w_half
      return w / 2
    end
    
    # Returns half of the height (rounded down). Useful for centering, etc
    def h_half
      return h / 2
    end
    
    # Blits to target a according to a 9 part scaling alorithm. 
    # This splits the bitmap in 9 parts, 
    # keeping the 4 corners unscaled, but scaling the 4 edges and the center  
    # according to the desired size. The 4 corners should be rectangles of
    # identical size corner_w by corner_h in the original image.
    # new_w and new_h are the new dimensions the new image should have. 
    # This is useful for GUI backgrounds.
    def blitscale9(xx, yy, src, new_w, new_h, corner_w = nil, corner_h = nil)
      corner_w  ||= src.h / 16
      corner_h  ||= corner_w
      mid_src_w= src.w - (corner_w * 2) 
      mid_src_h= src.h - (corner_h * 2)
      mid_dst_w= new_w - (corner_w * 2) 
      mid_dst_h= new_h - (corner_h * 2)
      # top parts
      leftx   = xx # left  
      rightx  = xx + mid_dst_w + corner_w # right 
      middlex = xx + corner_w # middle
      dsty    = yy
      self.blitscale(middlex, dsty, mid_dst_w, corner_h, 
                       src, corner_w, 0 , mid_src_w, corner_h)
      self.blitpart(leftx, dsty,   src, 0,  0, corner_w, corner_h)
      self.blitpart(rightx, dsty, 
                      src, src.w - corner_w, 0, corner_w, corner_h)
      dstmidh = new_h - (2 * corner_h)
      dstmidw = new_w - (2 * corner_w)
      srcmidh = src.h - (2 * corner_h)
      srcmidw = src.w - (2 * corner_w)
      
      dsty    = yy + corner_h
      self.blitscale(leftx, dsty, corner_w, dstmidh, 
                      src, 0, corner_h, corner_w, srcmidh)
      self.blitscale(rightx, dsty, corner_w, dstmidh, 
                      src, src.w - corner_w, corner_h, corner_w, srcmidh)
      self.blitscale(middlex,  dsty, dstmidw, dstmidh, 
                      src, corner_w, corner_h, mid_src_w, mid_src_h)
                      
      dsty = yy + corner_h + mid_dst_h
      self.blitscale(middlex, dsty, mid_dst_w, corner_h, 
                       src, corner_w, src.h - corner_h , mid_src_w, corner_h)
      self.blitpart(leftx, dsty, src, 0,  src.h - corner_h, corner_w, corner_h)
      self.blitpart(rightx, dsty, 
                      src, src.w - corner_w, src.h - corner_h, corner_w, corner_h)
    end
    
    
    # draws a filled frame of the given thickness on the screen.
    # The outer size of the frame will be ww and hh.
    # border color is fg, background color is bg. bg is not drawn 
    # if omitted or nil.
    def frame(xx, yy, ww, hh, tt, fg, bg=nil)  
      # top ...
      self.slab(xx, yy, ww, tt, fg)
      # bottom ...   
      self.slab(xx, yy + hh - tt, ww, tt, fg)
      # left 
      self.slab(xx, yy, tt, hh, fg)
      # right
      self.slab(xx + ww - tt, yy, tt, hh, fg)

      if bg 
        self.slab(xx + tt, yy + tt, ww - (tt*2), hh - (tt*2), bg)
      end  
    end
    
    # Draws a rounded frame 
    def roundframe(xx, yy, ww, hh, tt, fg, bg=nil)
      # top ...
      self.slab(xx + tt, yy, ww - tt * 2, tt, fg)
      # bottom ...   
      self.slab(xx + tt, yy + hh - tt, ww - tt * 2, tt, fg)
      # left 
      self.slab(xx , yy + tt, tt, hh - tt * 2, fg)
      # right
      self.slab(xx + ww - tt, yy + tt, tt, hh - tt * 2, fg)
      
      # tl corner
      self.disk(xx , yy, tt * 2, fg)
      # tr corner
      self.disk(xx + ww - tt * 2, yy, tt * 2, fg)
      # bl corner
      self.disk(xx , yy + hh - tt * 2 - 1, tt * 2, fg)
      # br corner
      self.disk(xx + ww - tt * 2, yy + hh - tt * 2 - 1, tt * 2, fg)
      
      if bg 
        self.slab(xx + tt, yy + tt, ww - (tt*2), hh - (tt*2), bg)
      end  
    end
    
    MODE_LOOKUP = { :alpha    => Gari::Image::ALPHA    , 
                    :colorkey => Gari::Image::COLORKEY ,
                    :solid    => Gari::Image::SOLID    
                  }
    def self.sym_to_mode(type)
      if type.respond_to? :to_sym
        return MODE_LOOKUP[type.to_sym]
      end 
      return type.to_i
    end
                    
    alias :old_optimize :optimize
    
    # Copies this image to an image optimized for display on the game screen. 
    # type is the mode, which may be :alpha, :solid, or :colorkey
    # If colorkey is given, it must be a dye obtained from Color#dye
    # after optimizing the color. 
    def optimize(type, colorkey = 0)
      old_optimize(self.class.sym_to_mode(type), colorkey)
    end
    
    class << self
      alias :old_new :new
      # Creates a new image with withd w and height h. The color depth is d 
      # bits per pixel. m is the mode, which may be :alpha or :solid 
      def new(w, h, d, m) 
        return old_new(w, h, d, sym_to_mode(m))
      end
    end
    
    def copy_rectangle(x, y, w, h)
      img = self.class.new(w, h, 16, :alpha)
      img.blitpart(0, 0, self, x, y, w, h)
      return img
    end
    
  end # class Image  
end # module Gari

