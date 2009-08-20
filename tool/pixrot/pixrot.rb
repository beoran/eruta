#!/usr/local/bin/ruby1.9
# Tool to rotate images pixel perfectly by 45 degrees
# this rotation algorithm does not maintain proportions, but it does keep
# every pixel at exactly the same color.

require 'RMagick'

# Chess king (or Chebyshev) distance between two points
def dchess(x1, y1, x2, y2)
  return [ (x2 - x1).abs, (y2 - y1).abs].max
end

def pixrot_45(x, y, cx, cy)
  dstart = dchess(x, y, cx, cy)
  # Chess king (or Chebyshev) distance between point and center determines 
  # the 'pixel rectangle' this pixel is on, as wel as how many "steps" the 
  # pixel has to move on this pixel rectangle shell. 
  steps = dstart
  # For an angle of 45 degrees, the steps equal the chebychev distance
  newx = x 
  newy = y
  steps.times do
    # mlove the point one step 
    if newx == newy # a corner point / case
      if newx > cx
        newy += 1
        # move one up beyond the center  
      elsif newx < cx 
        newy -= 1
      else # the center point strays unchanged 
         newx = x
         newy = y
      end
    elsif (newy - cy) == -dstart # on the upper rim
      newy -= 1
    elsif  (newy - cy) == -dstart # on the lower rim
      newy += 1
    elsif (newx - cx) == -dstart # on the left rim
      newx += 1
    else # on the right rim
      newy -= 1
    end 

    # If we get here, the point doesn't rotate
  end 
  return newx, newy
end

imagelist = Magick::ImageList.new('tryrot.png')
src       = imagelist[0]
dest      = src.copy
 
cx        = src.columns / 2 
cy        = src.rows    / 2
squa      = [cx * 2 + 1,  cy * 2 + 1].max 
dest.scale!(squa, squa) # Size should be odd, and square
dest.background_color = "black"
dest.erase! 


src.each_pixel do | pix, x, y | 
  newx, newy =  pixrot_45(x, y, cy + 1, cx + 1)
  next if newx <= 0 || newx >= dest.columns
  next if newy <= 0 || newy >= dest.rows
  dest.store_pixels( newx, newy, 1, 1, [pix]) 
end

# dest.display
dest.write('tryrot_out.png')
puts "done!"

