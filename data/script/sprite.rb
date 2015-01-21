

# Sprite constants 

# Flags of a Sprite, frame or layer. */
# Zero flag means sprite , frame, etc not in use. */
  SPRITE_EMPTY          = 0
# Sprite elemnt is in use if this is set, not if not. */ 
  SPRITE_ACTIVE         = 1
# Sprite element does not need to be updated. */
  SPRITE_FREEZE         = 2
# Direction flags */
  SPRITE_NO_DIRECTION   = 0
  SPRITE_SOUTH          = 1 << 8
  SPRITE_EAST           = 1 << 9
  SPRITE_NORTH          = 1 << 10
  SPRITE_WEST           = 1 << 11
  SPRITE_ALL            = SPRITE_SOUTH + SPRITE_EAST + SPRITE_NORTH + SPRITE_WEST

# /* Type of the sprite action. The numbers correspond with the 
#  * row in the liberated pixel cup sprite sheet compilation divided by 4, 
#  * which is used  placeholder art now. The real art will have the same structure 
#  * for it's sprite sheets.
#  * 
#  * To limit art requirements, emotions will be showed with emoticons and through
#  * the character portraits in the dialogues.
#  * 
#  */
  SPRITE_CAST           = 0
  SPRITE_STAB           = 1
  SPRITE_WALK           = 2
  SPRITE_SLASH          = 3
  SPRITE_SHOOT          = 4
  SPRITE_DOWN           = 5
  
#   /* The positions below are not in the LPC 
#    * sprite sheet but could be synthesized or used in the real art. */  
  SPRITE_RUN            = 6
  SPRITE_HURT           = 7
  SPRITE_STAND          = 8
  SPRITE_DEFEND         = 9
  

# /* Sprite layer suggested uses. */
  SPRITELAYER_BEHINDBODY = 0
  SPRITELAYER_BODY       = 1
  SPRITELAYER_HEAD       = 2
  SPRITELAYER_EYES       = 3
  SPRITELAYER_HAIR       = 4
  SPRITELAYER_HANDS      = 5
  SPRITELAYER_FEET       = 6
  SPRITELAYER_LEGS       = 7
  SPRITELAYER_TORSO      = 8
  SPRITELAYER_ACCESSORIES= 9
  SPRITELAYER_WEAPONS    = 10 


# some more OO wrappers for the low level sprite functionality
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
  
  def self.ulpcss_load(sprite_id, layer, name) 
    full_name = "image/ulpcss/#{name}"
    Eruta::Sprite.load_ulpcss sprite_id, layer, full_name
  end

  # Applies a tint to the given layer of the sprite.
  def tint(layer, r, g, b, a = 255)
    Eruta::Sprite.tint_rgba @id, layer, r, g, b, a
  end  
  
  # Applies a tint to the torso layer of the sprite.
  def tint_torso(r, g, b, a = 255)
    tint(SPRITELAYER_TORSO, r, g, b, a)
  end 
  
  # Applies a tint to the hair layer of the sprite.
  def tint_hair(r, g, b, a = 255)
    tint(SPRITELAYER_HAIR, r, g, b, a)
  end 
  
  # Loads an ULPCSS formatted sprite sheet as a layer.
  def load_ulpcss(layer, vpath)
    Sprite.ulpcss_load(@id, layer, vpath)
  end
  
  # Makes a new sprite possibly with the given ID, or otherwise tries to find a 
  # free id.
  def self.make(id=nil)
    id ||= self.get_unused_id
    return nil if !id
    return self.new(Eruta::Sprite.sprite_new(id))
  end
  
end


