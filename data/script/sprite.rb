

# Some more OO wrappers for the low level sprite functionality
class Sprite < Eruta::Sprite
  attr_reader :id
  
  def self.registry
    @registry ||= {}
  end
  
  def self.register(thing)
    @registry ||= {}
    @registry[thing.id] = thing
  end

  # Look up a Sprite in the Sprite registry
  def self.[](id)
    @registry[id]
  end

  
  def initialize(id) 
    @id = id
    self.class.register(self)
  end
    
  def self.get_unused_id
    29000.times do | i | 
      return i unless self.registry[i]
    end
    return nil
  end

  BUILTIN_LAYOUTS = {
    :ulpcss       => Eruta::Sprite::LOAD_ULPCSS_NORMAL,
    :ulpcss_slash => Eruta::Sprite::LOAD_ULPCSS_OVERSIZED_SLASH,
    :ulpcss_stab  => Eruta::Sprite::LOAD_ULPCSS_OVERSIZED_STAB,
  }
  
  def self.load_builtin(sprite_id, layer, name, layout = :ulpcss)
    ilayout = BUILTIN_LAYOUTS[layout]
    full_name = "image/ulpcss/#{name}"
    p "load_builtin", sprite_id, layer, full_name, ilayout 
    Eruta::Sprite.load_builtin sprite_id, layer, full_name, ilayout
  end
  
  # Loads a sprite sheet with built in layout as a layer.
  def load_builtin(layer, vpath, layout = :ulpcss)
    Sprite.load_builtin(@id, layer, vpath, layout)
  end

  # Loads a ULPCSS sprite sheet with built in layout as a layer.
  def load_ulpcss(layer, vpath)
    Sprite.load_builtin(@id, layer, vpath, :ulpcss)
  end

  # Loads a ULPCSS sprite sheet for an oversized slashing weapon
  # with built in layout as a layer.
  def load_ulpcss_slash(layer, vpath)
    Sprite.load_builtin(@id, layer, vpath, :ulpcss_slash)
  end

  # Loads a ULPCSS sprite sheet for an oversized stabbing weapon
  # with built in layout as a layer.
  def load_ulpcss_stab(layer, vpath)
    Sprite.load_builtin(@id, layer, vpath, :ulpcss_stab)
  end
  
  # Makes a new sprite possibly with the given ID, or otherwise tries to find a 
  # free id.
  def self.make(id=nil)
    id ||= self.get_unused_id
    return nil if !id
    return self.new(Eruta::Sprite.sprite_new(id))
  end
  
  # Returns the action ID for the given poe and direction for this sprite.
  def action_index_for(pose, direction)
    return Erute::Sprite.action_index_for(@id, pose, direction)
  end
  
end


