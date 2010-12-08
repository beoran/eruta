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
    def scale_9(target, xx, yy, new_w, new_h, corner_w = 16, corner_h = nil)
      corner_h  ||= corner_w      
      center_old_w= self.w - (corner_w * 2) 
      center_old_h= self.h - (corner_h * 2)
      center_new_w= new_w  - (corner_w * 2) 
      center_new_h= new_h  - (corner_h * 2) 
      # First blit the four border strips  
      # Top strip
      self.blitscale(target, xx + corner_w, yy      , corner_w    , 0, 
                      center_old_w, corner_h, center_new_w , corner_h)
      # Bottom strip
      self.blitscale(target  , xx + corner_w, yy + corner_h + center_new_h,
                      corner_w, corner_h + center_old_h, 
                      center_old_w, corner_h, center_new_w, corner_h)
      # Left strip
       self.blitscale(target  , xx , yy + corner_h,
                      0, corner_h, corner_w, center_old_h,  
                      corner_w, center_new_h)

      # Right strip
       self.blitscale(target  , xx + corner_w + center_new_w, yy + corner_h,
                       corner_w + center_old_w, corner_h, corner_w, center_old_h,  corner_w, center_new_h)                       
  
      # Now, blit the four corners
            
      # Top left to top left
      srcy        = srcx   = 0
      tary        = yy
      tarx        = xx
      self.blitpart(target, tarx, tary, srcx, srcy, corner_w, corner_h)
      # Top right to top right
      srcx        = self.w - corner_w 
      tarx        = xx + new_w  - corner_w      
      self.blitpart(target, tarx, tary, srcx, srcy, corner_w, corner_h)
      # Bottom left to bottom left
      srcx        = 0      
      srcy        = self.h - corner_h
      tarx        = xx
      tary        = yy + new_h  - corner_h       
      self.blitpart(target, tarx, tary, srcx, srcy, corner_w, corner_h)
      # Bottom right to bottom right
      srcx        = self.w - corner_w
      tarx        = xx + new_w  - corner_w
      self.blitpart(target, tarx, tary, srcx, srcy, corner_w, corner_h)
      
    # Next, blit the middle rectangle of self scaled to the new dimensions
    # XXX? Must it be made a bit bigger ythat it should for library reasons?
      self.blitscale(target, xx + corner_w , yy + corner_h ,
                      corner_w, corner_h, center_old_w, center_old_h, 
                      center_new_w, center_new_h)
   end
    
  end # class Image  
end # module Gari

