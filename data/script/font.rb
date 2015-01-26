  
# Models a font that has been loaded from disk and kept in storage
class Font < Store
  extend Store::Forward
  
  # Loads a truetype or opentype font
  def self.load_ttf(name, vpath, height, flags = 0)
    load_something(forward_name(name), vpath, self) do | nid |
      Eruta::Store.load_ttf_font(nid, vpath, height, flags=0)
    end 
  end
   
  # Loads a truetype or opentype font and stretches it
  def self.load_ttf_stretch(name, vpath, height, width, flags)
    load_something(forward_name(name), vpath, self) do | nid |
      Eruta::Store.load_ttf_stretch(nid, vpath, height, width, flags)
    end 
  end

  # Loads a bitmap font
  def self.load_bitmap(name, vpath)
    load_something(forward_name(name), vpath, self) do | nid |
      Eruta::Store.load_bitmap_font(nid, flags)
    end 
  end
  
  # Loads a bitmap font with flags 
  def self.load_bitmap_flags(name, vpath, flags)
    load_something(forward_name(name), vpath, self) do | nid |
      Eruta::Store.load_bitmap_font_flags(nid, vpath, flags)
    end 
  end

  # Loads a font. If the extension is .ttf or .otf, this calls
  # load_ttf or load_ttf_stretch, otherwise it calls load_bitmap or
  # load_bitmap_flags
  def self.load(name, vpath, *args)
    ext = vpath.split('.').last.downcase
    case ext
    when 'ttf', 'otf', 'afm', 'pfb', 'pfm', 'pcf', 'gz', 'sfd'
      if args.size >= 3
        height, width, flags = *args
        return load_ttf_strech(name, vpath, height, width, flags)
      else
        height, flags = *args
        return load_ttf(name, vpath, height, flags)      
      end 
    when 'bmp', 'pcx', 'tga', 'jpeg', 'jpg', 'png'
      if args.size >= 1
        flags = *args
        return load_bitmap_flags(name, vpath, flags)
      else
        return load_bitmap(name, vpath)      
      end 
    else
      raise "Unknown font file name extension."
    end
  end


  # Ascent height of the font.
  def ascent
    return Eruta::Store.font_ascent(@id)
  end 
  
  # Descent of the font.
  def descent
    return Eruta::Store.font_descent(@id)
  end 
  
  # Line height of the font.
  def line_height
    return Eruta::Store.font_line_height(@id)
  end 
  
  # Text dimensions of the given text in the font.
  def text_dimensions(text)
    return Eruta::Store.text_dimensions(@id, text)
  end 
  
  # Width of the given text in the font.
  def text_width(text)
    return Eruta::Store.text_width(@id, text)
  end


end  










