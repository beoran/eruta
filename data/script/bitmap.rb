
# Models a bitmap that has been loaded from disk and kept in storage
class Bitmap < Store
  extend Store::Forward

  # Loads a bitmap
  def self.load(name, vpath)
    load_something(forward_name(name), vpath, self) do | nid |
      Eruta::Store.load_bitmap(nid, vpath)
    end
  end
  
  # Loads bitmap with flags
  def self.load_flags(name, vpath, flags)
    load_something(forward_name(name), vpath, self) do | nid |
      Eruta::Store.load_bitmap_flags(nid, vpath, flags)
    end 
  end

  # Converts the mask color of this bitmapp expressed by r, g, b to transparent.
  def mask_to_alpha(r, g, b)
    return Eruta::Store.mask_to_alpha(@id, r, g, b)
  end
  
  # Converts the average of the sr, sg, sb of each pixel of the image to 
  # the given r, g, b, with with an alpha value equal to the aforementioned 
  # alpha. Useful to make images monochromatic.
  def average_to_alpha(r, g, b)
    return Eruta::Store.average_to_alpha(@id, r, g, b)
  end

  # Flags of the loaded bitmap.
  def flags
    return Eruta::Store.bitmap_flags(@id)
  end 
  
  # Width of the loaded bitmap.
  def width
    return Eruta::Store.bitmap_width(@id)
  end 
  
  # Height of the loaded bitmap.
  def height
    return Eruta::Store.bitmap_height(@id)
  end


end
