

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

  # Look up a thing in the thing registry
  def self.[](id)
    @registry[id]
  end

  def self.lookup(thing_id)
    return @registry[thing_id]
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
    id ||= self.get_unused_id
    return nil if !id
    return self.new(Eruta::Thing.thing_new(id, kind, x, y, z, w, h))
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

  def find_in_front(w = 32, h = 32, debug = false)
    x, y, w, h = self.rectangle_in_front(w, h)
    if debug
      if @search_box
        @search_box.position = [x - camera_x, y - camera_y]
        @my_box.position = [self.x - camera_x, self.y - camera_y]
      else
        @search_box = Graph.make_box(x - camera_x, y - camera_y, w, h, 0, 0)
        @search_box.visible          = true
        @search_box.border_color     = [255, 255, 0]
        @search_box.border_thickness = 1
        @my_box = Graph.make_box(self.x - camera_x, self.y - camera_y, self.w, self.h, 0, 0)
        @my_box.visible              = true
        @my_box.border_color         = [0, 255, 255]
        @my_box.border_thickness     = 1
      end
      puts "Rectangle in front:"
      p x, y , w, h, @search_box, camera_x, camera_y
    end
    return Thing.find_in_rectangle(x, y, w, h, @id)
  end
  
  # Forces the camera to track this thing. 
  def track
    camera_track(@id)
  end
                   
  # Sets the sprite id for this thing
  def sprite_id=(spid)
    Eruta::Thing.sprite_(@id, spid)
    @sprite_id = spid
    # Set sprite's default one shot actions
    one_shot_default()
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
        self.set_pose_direction_loop(pose, direction, Sprite::State::ACTION_ONESHOT)
      end
    end
    
    # the "Drop down" action is a one stop action in ULPCSS sprites.
    self.set_pose_direction_loop(Sprite::DOWN, Sprite::NO_DIRECTION, Sprite::State::ACTION_ONESTOP)
  end
  
  
   
  
  


end

