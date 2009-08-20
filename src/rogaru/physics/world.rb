# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 
require 'chipmunk'
   

module Rogaru
  module Physics
    class World
 
    autoload :Thing, 'rogaru/world/thing'
    autoload :Mobile, 'rogaru/world/mobile'
    autoload :Actor, 'rogaru/world/actor'
    
    attr_reader   :space
    attr_accessor :things
    attr_accessor :mobiles

    def initialize
      @space          = CP::Space.new()
      @space.gravity  = CP::Vec2.new(0, 0)
      @space.damping  = 0.0      
      @space.resize_static_hash(128.0, 500 )
      @space.resize_active_hash(128.0, 500 )
      @space.add_collision_func(:mobile, :thing) do  |shape1, shape2|
          shape1.body.v.x = 0
          shape1.body.v.y = 0
          shape2.body.v.x = 0
          shape2.body.v.y = 0
      end

      @things     = []
      @mobiles    = []
      @all        = []
    end
    
    # Adds immobile thing to the space 
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
    

    # Draws on-screen diagnostics like bounding boxes and 
    # object indexes of objects in this Space on the screen. 
    def draw(screen, scroll_x, scroll_y, font = nil)  
      @drawindex = false
      @drawwide  = false
      offset_x = -scroll_x
      offset_y = -scroll_y
      ttcol = [250, 250, 250]
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
    
    # Adds one layer from a tile map to the world simulation
    def add_map_layer(layer, layer_index)
      layer.each_tile do |tile, x, y|
       if tile && (!tile.walk)
         w = layer.tileset.tilewide - 2
         h = layer.tileset.tilehigh - 2
         self.new_thing(x + 1, y + 1, layer_index, w, h)
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
       @space.step(time) 
      # @entities.first.body.update_velocity(@space.gravity, 0, @dt)
      # @entities.first.body.update_position(@dt)
      for thing in @all do
          # thing.body.update_velocity(vec2(0,0), 0.5, time)
          # thing.body.reset_forces
      end
    end

    end # class World
  end # module Physics
end # module Rogaru
