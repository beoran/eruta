

# some more OO wrappers for the low level thing functionality
class Thing < Eruta::Thing
  extend Registry 
  
  attr_reader   :id
  attr_reader   :sprite_id
  attr_reader   :sprite
  attr_accessor :parent
  attr_reader   :name


  def initialize(id, name) 
    @id           = id
    @name         = name || "thing_#{id}"
    # child things, keyed by id
    @children     = {}
    # parent thing 
    @parent       = nil
    self.sprite   = Sprite.make(("sprite_" + @name.to_s).to_sym)
    self.class.register(self)
  end
  
  def self.make(name, kind, x, y, z, w, h)
    id = Eruta::Thing.thing_new(kind, x, y, z, w, h)
    p "Making thing #{name} #{id}"
    return nil if id < 0
    return self.new(id, name)
  end
  
  # adds child as a child of self
  def add_child(child) 
    child.parent = self
    @children[child.id] = child
  end
  

  def self.find_in_rectangle(x, y, w, h, self_id)
    ids =  Eruta::Thing.find_in_rectangle(x, y, w, h)
    ids.delete(self_id)
    result = ids.map { |id| self.lookup(id) }
  end

  # Forwards methods to Eruta::Thing(@id, * args)
  def self.forward_thing(name)
    clean_name = name.to_s.gsub("=", "_").gsub('?', '_p').gsub('!', '_bang') 
    method_name = "#{clean_name}".to_sym
    define_method(name) do |*args|
      Eruta::Thing.send(method_name, @id, *args)
    end
  end

  def v
    return Eruta::Thing.v @id
  end
  
  def v=(point)
    return Eruta::Thing.v_ @id, *point
  end
  

  forward_thing :x
  forward_thing :y
  forward_thing :cx
  forward_thing :cy
  forward_thing :w
  forward_thing :h
  forward_thing :z
  forward_thing :direction
  forward_thing :pose
  
  def is_facing?(direction)
    dir_now = self.direction
    return ((dir_now & direction) == direction)
  end

  # Returns the x, y w and h of a rectangle "in front"
  # of the direction the thing is facing.
  def rectangle_in_front(w = 32, h = 32)
    dire = self.direction
    cx   = self.cx
    cy   = self.cy
    if is_facing?(Eruta::Sprite::NORTH)
      cy -= ( h / 2.0 + self.h / 2.0)
    elsif is_facing?(Eruta::Sprite::EAST)
      cx += ( w / 2.0 + self.w / 2.0)
    elsif is_facing?(Eruta::Sprite::SOUTH)
      cy += ( h / 2.0 + self.h / 2.0)
    elsif is_facing?(Eruta::Sprite::WEST)
      cx -= ( w / 2.0 + self.w / 2.0)
    end
    x = cx - (w / 2.0)
    y = cy - (h / 2.0)
    return x, y, w, h
  end

  def find_in_front(w = 32, h = 32)
    x, y, w, h = self.rectangle_in_front(w, h)
    return Thing.find_in_rectangle(x, y, w, h, @id)
  end
  
  # Forces the camera to track this thing. 
  def track
    camera_track(@id)
  end
                   
  # Sets the sprite for this thing 
  def sprite=(sprite)
    if (@sprite)
      @sprite = @sprite.delete
    end

    @sprite        = sprite      
    Eruta::Thing.sprite_(@id, @sprite.id)
    @sprite_id = @sprite.id
    # Set sprite's default one shot actions
    one_shot_default()
    puts "set sprite #{@sprite.id}"
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
  
  
  # Applies a tint to the given layer of the thing's sprite.
  def tint(layer, r, g, b, a = 255)
    Eruta::Thing.tint_rgba @id, layer, r, g, b, a
  end  
  
  # Applies a tint to the torso layer of the thing's sprite.
  def tint_torso(r, g, b, a = 255)
    tint(Eruta::Sprite::Layer::TORSO, r, g, b, a)
  end 
  
  # Applies a tint to the hair layer of the thing's sprite.
  def tint_hair(r, g, b, a = 255)
    tint(Eruta::Sprite::Layer::HAIR, r, g, b, a)
  end 
  
  # Hides the thing's given sprite layer
  def hide_layer(layer)
    Eruta::Thing.hide_layer @id, layer, 1
  end  
 
  # Unhides the thing's given sprite layer
  def show_layer(layer)
    Eruta::Thing.hide_layer @id, layer, 0
  end
  
  # Returns true if the layer is hidden false if not
  def layer_hidden?(layer)
    Eruta::Thing.layer_hidden?(@id, layer)
  end
    
  # Sets the loop mode of the Thing's action.
  def set_action_loop(action, loop)
    Eruta::Thing.set_action_loop(@id, action, loop)
  end
  
  # Gets the loop mode of the Thing's action.
  def get_action_loop(action)
    Eruta::Thing.get_action_loop(@id, action)
  end

  # Sets the loop mode of the Thing's pose and direction sprite.
  def set_pose_direction_loop(pose, direction, loop)
    Eruta::Thing.set_pose_direction_loop(@id, pose, direction, loop)
  end
  
  # Gets the loop mode of the Thing's pose and direction sprite.
  def get_pose_direction_loop(pose, direction)
    Eruta::Thing.get_pose_direction_loop(@id, pose, direction)
  end
  
  # Gets the status of the Thing's action.
  def action_done?(action)
    Eruta::Thing.action_done?(@id, action)
  end
    
  # Sets the action to loop mode.
  def loop_action(action)
    self.set_action_loop(action, Eruta::Sprite::State::ACTION_LOOP)
  end
  
  # Sets the action to one shot mode.
  def one_shot_action(action)
    self.set_action_loop(action, Sprite::State::ACTION_ONESHOT)
  end
  
  # Sets actions that should probably be one-shot to one shot.
  def one_shot_default()
    poses = [ Sprite::SLASH, Sprite::CAST,  
              Sprite::STAB, Sprite::SHOOT, 
              Sprite::HURT ] 
    directions = [ Sprite::NORTH, Sprite::EAST, Sprite::SOUTH, Sprite::WEST ] 
    
    poses.each do | pose |
      directions.each do | direction |
        p "Pose"
        p self.set_pose_direction_loop(pose, direction, Sprite::State::ACTION_ONESHOT)
      end
    end
    
    # the "Drop down" action is a one stop action in ULPCSS sprites.
    res = self.set_pose_direction_loop(Sprite::DOWN, Sprite::ALL_DIRECTIONS, Sprite::State::ACTION_ONESTOP)
    p "Drop Down"
    p res
  end
  
  # Sets a flag of the main hull (physical presence) of the thing
  forward_thing :set_hull_flag
  
  # Unsets a flag of the main hull (physical presence) of the thing
  forward_thing :unset_hull_flag

  # Sets all flags of the main hull (physical presence) of the thing
  forward_thing :hull_flags=
  
  # Gets all flags of the main hull (physical presence) of the thing
  forward_thing :hull_flags
  
  # Loads a sprite sheet with built in layout as a layer
  # of the sprite of this thing.
  def load_builtin(layer, vpath, layout = :ulpcss)
    return nil unless @sprite
    @sprite.load_builtin(layer, vpath, layout = :ulpcss)
  end

  # Loads a ULPCSS sprite sheet with built in layout as a layer 
  # of the sprite of this thing.
  def load_ulpcss(layer, vpath)
    return nil unless @sprite
    @sprite.load_ulpcss(layer, vpath)
  end

  # Loads a ULPCSS sprite sheet for an oversized slashing weapon
  # with built in layout as a layer of the sprite of this thing..
  def load_ulpcss_slash(layer, vpath)
    return nil unless @sprite
    @sprite.load_ulpcss_slash(layer, vpath)
  end

  # Loads a ULPCSS sprite sheet for an oversized stabbing weapon
  # with built in layout as a layer of the sprite of this thing..
  def load_ulpcss_stab(layer, vpath)
    return nil unless @sprite
    @sprite.load_ulpcss_stab(layer, vpath)
  end
  
  # React to sprite event
  def on_sprite(spriteid, pose, direction, kind)
    p "Sprite event for #{name}"
  end
  
  
  
  
  
  
  

end

