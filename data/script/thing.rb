

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

