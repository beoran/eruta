# encoding: utf-8
# World models the physical world contained within a map.
# Note on use of layers: layers are used in pairs. Layer 0 is everything that 
# should appear under actors/player's feet, layer 1 is everything the
# player/actor may interact with (even though it may also be underfoot!), 
# such as walls, stairs, or zones.
# Layer 2 and 3 are used similarly, but are always above layer 0 and 1.   
# Note on stairs: stairs are created by overlaying a stair up on level 1 
# over a background on level 0, and a stair down on level 3 with a 
# background on level 2.   
 
require 'chipmunk'
   

module Rogaru

  # World models the pysical aspects of the game world, 
  # currenly using Chipmunk
  
  class World
    VECTOR_ZERO = vec2(0, 0)
    INFINITY    = 1.0 / 0.0
    
    # Extend CP's Shape classes to allow tracing back to the World::Thing 
    # the CP::Shape belongs to
    class CP::Shape::Poly
      attr_accessor :thing
    end
    
    class CP::Shape::Circle
      attr_accessor :thing
    end
    
    class CP::Shape::Segment
      attr_accessor :thing
    end
    
    STATIC_BODY = CP::Body.new(INFINITY, INFINITY)

 
    autoload :Thing , 'rogaru/world/thing'
    autoload :Wall  , 'rogaru/world/wall'
    autoload :Stair , 'rogaru/world/stair'
    autoload :Mobile, 'rogaru/world/mobile'
    autoload :Actor , 'rogaru/world/actor'

    attr_reader   :space
    attr_accessor :things
    attr_accessor :mobiles
    
    # Effect for collision with a stair 
    def stair_effect(mobile, stair)
      if stair.thing.direction == :up && mobile.thing.z < 2
        mobile.thing.z += 2
      elsif stair.thing.direction == :down && mobile.thing.z > 2
        mobile.thing.z -= 2 
      end
    end

    def initialize
      # CP.collision_slop = 0.01
      # Reduce collision sloppiness? Higher slop may be faster, 
      # though less accurate. Leave it as is for now.
      @space            = CP::Space.new()
      @space.gravity    = vec2(0, 0)
      @space.damping    = 0.0      
      @space.resize_static_hash(128.0, 500)
      @space.resize_active_hash(128.0, 500)

      
      @space.add_collision_func(:any, :any) {
        |shape_1, shape_2|
        p shape_1, shape_2 if shape_1.is_a?(Stair) || shape_2.is_a?(Stair) 
        # p "Any: #{shape_1} #{shape_2}"
        true
      }
      @space.add_collision_func(:actor, :wall) {
        |shape_1, shape_2|
        # p "Actor to wall: #{shape_1} #{shape_2}"
        true
      }
      
      @space.add_collision_func(:any, :zone) {
        |shape_1, shape_2|
        false
      }
       
      # Stairs are areas that move the player character between different layers. 
      @space.add_collision_func(:actor, :stair) {
        |actor, stair|
        p "Actor on stair!", stair.thing.direction
        stair_effect(actor, stair)
        # Let the actor go up or down the stairs.
        p "Actor z is now: #{actor.thing.z}"
        false
      }
      
      # Refuse interaction bewteen chairs and mobile objects. 
      @space.add_collision_func(:mobile, :stair) {
        |mobile, stair|
        true # Simply don't allow collision.
      }


      
      
      @space.add_collision_func(:mobile, :thing) do  |shape1, shape2|
=begin      
        shape1.body.v.x = 0
        shape1.body.v.y = 0
        shape2.body.v.x = 0
        shape2.body.v.y = 0
=end        
      end
      @things     = []
      @mobiles    = []
      @walls      = []
      @actors     = []
      @stairs     = []
      @zones      = []
      @all        = []
    end
    
    
    
    # Adds wall to the space. 
    def add_wall(wall)
      @walls << wall
      @space.add_static_shape(wall.shape)
      # @space.add_body(thing.body)
      return wall
    end
    
    # Adds immobile thing to the space. 
    def add_thing(thing)
      @things << thing
      @all    << thing
      # @lookup.store(thing)
      @space.add_static_shape(thing.shape)
      @space.add_body(thing.body)
      thing.index = @all.size - 1
      return thing
    end
  
    # Adds mobile to the space.
    def add_mobile(mobile)
      @mobiles << mobile
      @all     << mobile
      mobile.index = @all.size - 1
      @space.add_shape(mobile.shape)
      @space.add_body(mobile.body)
      return mobile
    end
    
     # Adds a stair to the space. 
    def add_stair(stair)
      @stairs << stair
      @space.add_static_shape(stair.shape)
      return stair
    end

    
    # Adds new wall thing to the space
    def new_wall(x, y, z, w, h)
      return add_wall(Wall.new(self, x, y, z, w, h))  
    end
    
    # Adds new immobile thing to the space
    def new_thing(x, y, z, w, h)
      return add_thing(Thing.new(self, x, y, z, w, h))  
    end
    
    # Adds new mobile to the space (that is, a thing that may be moved)  
    def new_mobile(x, y, z, w, h)
      return add_mobile(Mobile.new(self, x, y, z, w, h)) 
    end
  
    # Adds new actor to the space (that is, a thing that may move autonomously 
    # or that is controlled.)
    def new_actor(x, y, z, w, h)
      return add_mobile(Actor.new(self, x, y, z, w, h)) 
    end
    
    # Adds a new stair to the space
    def new_stair(x, y, z, w, h, direction = :up)
      return add_stair(Stair.new(self, x, y, z, w, h, INFINITY, INFINITY, direction))
    end
  
    # Draws on-screen diagnostics like bounding boxes and 
    # object indexes of objects in this World on the screen. 
    def draw(screen, scroll_x, scroll_y, font = nil)  
      @drawindex = false
      @drawwide  = false
      offset_x = -scroll_x
      offset_y = -scroll_y
      ttcol = [250, 250, 250]
      @walls.each  do 
          |w| w.render(screen, offset_x, offset_y)
      end
      @stairs.each  do 
          |w| w.render(screen, offset_x, offset_y)
      end
      @things.each  do 
          |t| t.render(screen, offset_x, offset_y)
          if font and @drawindex
            font.draw(screen, t.index.to_s, t.x + 5 + offset_x, t.y + 5 + offset_y, ttcol)
          end           
      end
      @mobiles.each do |m| 
          m.render(screen, offset_x, offset_y) 
          if font and @drawindex
            font.draw(screen, m.index.to_s, m.x + 5 + offset_x, m.y + 5 + offset_y, ttcol)
          end           
          if m.index == 0 and @drawwide
            points = @lookup.overlap(m)          
            for point in points do
              x, y = * point
              x   = x * @lookup.xgridsize
              y   = y * @lookup.ygridsize
              screen.draw_box(x + offset_x, y + offset_y, @lookup.xgridsize, @lookup.ygridsize, [255,127,0]) 
            end
          end
      end
    end
    
    # Adds one layer from a tile map to the world simulation.
    def add_map_layer(layer, layer_index)
      layer.each_tile do |tile, x, y|
        if tile
         w = layer.tileset.tilewide - 2
         h = layer.tileset.tilehigh - 2
          if (!tile.walk)
            self.new_wall(x + 1, y + 1, layer_index, w, h)
          elsif (tile.up)
            p "Added stair up! #{x} #{y}, #{layer_index} #{tile}"
            self.new_stair(x + 1, y + 1, layer_index, w, h, :up)  
          elsif (tile.down)
            p "Added stair down!"
            self.new_stair(x + 1, y + 1, layer_index, w, h, :down)
          end
        end
      end
    end
  
    # Adds a tile map to the world simulation
    def add_map(map)
      map.each_layer do |layer_index, layer| 
        add_map_layer(layer, layer_index)
      end
    end
  
    def update(time=1.0)
      @mobiles.each { |actor| actor.update(time) } 
      @space.step(time)
      # @entities.first.body.update_velocity(@space.gravity, 0, @dt)
      # @entities.first.body.update_position(@dt)
      for thing in @all do
          # thing.body.update_velocity(vec2(0,0), 0.5, 1.0)
          thing.body.reset_forces
      end
    end

  end # module Physics
end # module Rogaru
