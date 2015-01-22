# some more OO wrappers for the low level store functionality

# How to organise the game state, loaded data, etc in a neat way?
# Use a central repository or per type/class repositories???
# -> use per type registries for ease of use.
# In a central repository it's neccessary to do something like
# Repo[:font_tuffy]
# withe with per-type repos I can say
# Font[:tuffy] with is more compact and clear.
# Store items will have to be maintained both in
# Store by ID and in the relevant subclass  

script 'registry.rb'

class Store
  attr_reader :id
  attr_reader :name

  extend Registry
  
  # Initialize a storage item.
  def initialize(id, name) 
    @id   = id
    @name = name
    self.class.register(self, self.id, self.name)
  end

  # Load helper
  def self.load_something(name, vpath, klass = nil, &block)
    id    = Eruta::Store.get_unused_id(1000)
    return nil if id < 0
    res = block.call(id)
    return nil unless res
    klass ||= self
    return klass.new(id, name)
  end
  
  # loads a bitmap
  def self.load_bitmap(name, vpath)
    load_something(name, vpath) do | nid |
      Eruta::Store.load_bitmap(nid, vpath)
    end
  end
  
  # Loads bitmap with flags
  def self.load_bitmap_flags(name, vpath, flags)
    load_something(name, vpath) do | nid |
      Eruta::Store.load_bitmap_flags(nid, vpath, flags)
    end 
  end

  # Loads an audio stream
  def self.load_audio_stream(name, vpath, buffer_count=8, samples=8000)
    load_something(name, vpath) do | nid |
      Eruta::Store.load_audio_stream(nid, vpath, buffer_count, samples)
    end 
  end
  
  # Loads an audio sample 
  def self.load_sample(name, vpath)
    load_something(name, vpath) do | nid |
      Eruta::Store.load_sample(nid, vpath)
    end 
  end
   
  # Loads a truetype or opentype font
  def self.load_ttf_font(name, vpath, height, flags = 0)
    load_something(name, vpath) do | nid |
      Eruta::Store.load_ttf_font(nid, vpath, height, flags=0)
    end 
  end
   
  # Loads a truetype or opentype font and stretches it
  def self.load_ttf_stretch(name, vpath, height, width, flags)
    load_something(name, vpath) do | nid |
      Eruta::Store.load_ttf_stretch(nid, vpath, height, width, flags)
    end 
  end

  # Loads a bitmap font 
  def self.load_bitmap_font(name, vpath)
    load_something(vpath, name, id) do | nid |
      Eruta::Store.load_bitmap_font(nid, flags)
    end 
  end
  
  # Loads a bitmap font with flags 
  def self.load_bitmap_font(name, vpath, flags)
    load_something(name, vpath) do | nid |
      Eruta::Store.load_bitmap_font_flags(nid, vpath, flags)
    end 
  end
  
  # Loads a tile map
  def self.load_tilemap(name, vpath)
    load_something(name, vpath) do | nid |
      Eruta::Store.load_tilemap(nid, vpath)
    end 
  end
  
  # type numbers 
  NONE          = 0
  BITMAP        = 1 
  FONT          = 2 
  AUDIO_STREAM  = 3 
  SAMPLE        = 4
  CONFIG        = 5
  OTHER         = 8
  TILEMAP       = 9

  
  # The type number of the stored object 
  def kind
    return Eruta::Store.kind(@id)
  end
  
  # Converts the mask color expressed by r, g, b to transparent.
  def mask_to_alpha(r, g, b)
    return Eruta::Store.mask_to_alpha(@id, r, g, b)
  end
  
  # Converts the average of the sr,sg,sb of each pixel of the image to 
  # the given r, g, b, with with an alpga value equal to the aforementioned 
  # alpha.
  def average_to_alpha(r, g, b)
    puts "ata #{@id}"
    return Eruta::Store.average_to_alpha(@id, r, g, b)
  end
  
  # Drops this object from storage
  def drop!
    res = Eruta::Store.drop(@id)
    self.class.unregister(self)
    return res
  end
  
  # Flags, if self is a loaded bitmap.
  def bitmap_flags
    return Eruta::Store.bitmap_flags(@id)
  end 
  
  # Width, if self is a loaded bitmap.
  def bitmap_width
    return Eruta::Store.bitmap_width(@id)
  end 
  
  # Height, if self is a loaded bitmap.
  def bitmap_height
    return Eruta::Store.bitmap_height(@id)
  end 
  
  # Ascent, if self is a loaded font.
  def font_ascent
    return Eruta::Store.font_ascent(@id)
  end 
  
  # Descent, if self is a loaded font.
  def font_descent
    return Eruta::Store.font_descent(@id)
  end 
  
  # Line height if self is a loaded font.
  def font_line_height
    return Eruta::Store.font_line_height(@id)
  end 
  
  # Text dimensions, if self is a loaded font.
  def text_dimensions(text)
    return Eruta::Store.text_dimensions(@id, text)
  end 
  
  # Text width if self is a loaded font.
  def text_width(text)
    return Eruta::Store.text_width(@id, text)
  end

  # extend this module to makea class forward it's load, drop! and lookup calls 
  module Forward
    def forwarded_name(name)
      return self.to_s.downcase + '_' + name.to_s
    end
    
    def [](name)
      return Store[forwarded_name(name)]
    end    
  end
  
end


