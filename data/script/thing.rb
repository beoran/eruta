

# some more OO wrappers for the low level thing functionality
class Thing < Eruta::Thing
  attr_reader :id
  attr_reader :sprite_id
  attr_reader :sprite
  
  def self.registry
    @registry ||= {}
  end
  
  def self.register(thing)
    @registry ||= {}
    @registry[thing.id] = thing
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
  
  def self.make(kind, x, y, z, w, h, id = nil)
    puts "make"
    id ||= self.get_unused_id
    return nil if !id
    return self.new(Eruta::Thing.thing_new(id, kind, x, y, z, w, h))
  end
  
  def v
    Eruta::Thing.v(@id)
  end
  
  def v=(p)
    Eruta::Thing.v_(@id, *p)
  end
  
  # Forces the camera to track this thing. 
  def track
    camera_track(@id)
  end
                   
  # Sets the sprite id for this thing
  def sprite_id=(spid)
    Eruta::Thing.sprite_(@id, spid)
    @sprite_id = spid
  end
  
  # Sets the sprite for this thing 
  def sprite=(sprite)
    self.sprite_id = (sprite.id)
    @sprite        = sprite
    puts "set sprite #{sprite.id}"
  end
  
                   
  # Sets the pose for this thing
  def pose=(pose_id)
    Eruta::Thing.pose_(@id, pose_id)
  end
  
  def direction=(dir_id)
    Eruta::Thing.direction_(@id, dir_id)
  end
  
  def self.actor=(new_actor)
    @actor = new_actor
  end

  def self.actor
    @actor
  end

end

