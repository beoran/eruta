

# Some more OO wrappers for the low level sprite functionality
class Sprite < Eruta::Sprite
  extend Registry
    
  attr_reader :id
  attr_reader :name
  
  def initialize(id, name)
    @id     = id
    @name   = name
    self.class.register(self)
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
  
  # Makes a new sprite posibly with the given ID, or otherwise tries to find a 
  # free id.
  def self.make(name)
    id = Eruta::Sprite.sprite_new()
    p "Make sprite id #{id}"
    return nil if id < 0 
    return self.new(id, name)
  end
  
  # Returns the action ID for the given pose and direction for this sprite.
  def action_index_for(pose, direction)
    return Erute::Sprite.action_index_for(@id, pose, direction)
  end
  
  # Deletes the underlying sprite. Don't use it anumore afterward!
  def delete
    Eruta::Sprite.delete(@id)
    @id = nil
    return nil
  end
  
end


