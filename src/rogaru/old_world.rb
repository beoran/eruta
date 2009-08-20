# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 
require 'chipmunk'

module Rogaru
  module Physics
    
    VECTOR_ZERO = CP::Vec2.new(0.0, 0.0)
    INFINITY    = 1.0 / 0.0
    STATIC_BODY = CP::Body.new(INFINITY, INFINITY)
    
    class Entity
      include CP
      attr_reader   :body
      attr_reader   :shape
      attr_accessor :static
      
      def initialize(x, y, wide, high, layer =0, mass=1.0)
        @wide   = wide.to_f
        @high   = high.to_f
        @halfw  = wide / 2.0
        @halfh  = high / 2.0
        r1      = vec2(0      , 0       )
        r2      = vec2(0      , @high   )
        r3      = vec2(@wide  , @high   )
        r4      = vec2(@wide  , 0       )
        delta   = vec2( @halfw,  @halfh )
        verts   = [r1, r2, r3, r4]
        @static = (@mass == INFINITY)
        # @moment = @static ? INFINITY : moment_for_poly(mass, verts, delta)
        @moment = INFINITY # disable rotations 
        @body   = CP::Body.new(mass, @moment)
        @shape  = CP::Shape::Poly.new(@body, verts,  delta)
        @shape.u= 1.0 * mass         
        @body.p = vec2(x.to_f, y.to_f)
        
        @shape.layers = 1 << layer
        # Rectangular shape.
      end
      
      def layers=(*args)
        @layers = args.inject(0) {  |a, r| a + r }
      end
      
      def draw(screen, scroll_x=0, scroll_y=0)
        pos = @body.p - vec2(scroll_x, scroll_y)
        # screen position
        begin
        c1  = [pos.x.to_i, pos.y.to_i]
        c2  = [pos.x.to_i + @wide, pos.y.to_i + @high]
        col = [255, 255, 255]
        screen.draw_box(c1, c2, col)
        rescue
            p pos
        end
      end
      
      def apply_force(xf, yf)
        @body.apply_force(vec2(xf.to_f, yf.to_f), VECTOR_ZERO)
      end
      
      def apply_impulse(xf, yf)
        @body.apply_impulse(vec2(xf.to_f, yf.to_f), VECTOR_ZERO)
      end
      
      def self.new_static(x, y, wide, high, layer = 0)
          res  = self.new(x,y,wide,high, layer, INFINITY)
          res.static = true
          return res
      end
      
    end
    
    class World
      
      attr_reader :space
      def initialize
        @space          = CP::Space.new()
        # @space.gravity  = VECTOR_ZERO
        @space.gravity  = CP::Vec2.new(0.0, 0.0)
        @substep        = 6.0
        @dt             = 1.0 / 60.0
        @space.damping  = 0.0
        @space.resize_static_hash(40.0, 5000.0 )
        @entities       = []
      end
      
      def add(entity)
        @entities << entity
        @space.add_body(entity.body)
        if(entity.static)          
          @space.add_static_shape(entity.shape)          
        else
          @space.add_shape(entity.shape)
        end
      end
      
      def update
        # @entities.first.body.update_velocity(@space.gravity, 0, @dt)
        # @entities.first.body.update_position(@dt)
        
        for i in (0...@substep) do
          @space.step(@dt)                
        end
        
        for ent in @entities do
          ent.body.reset_forces
        end          
      end
      
       def draw(screen, scroll_x=0, scroll_y=0)
        for ent in @entities do
          ent.draw(screen, scroll_x, scroll_y)
        end          
      end
     
      def add_map_layer(layer, layer_index)
        layer.each_tile do |tile, x, y|
         unless tile.nil? || tile.walk
           tile_ent = Entity.new_static(x, y, layer.tilewide, layer.tilehigh, layer_index)
           self.add(tile_ent)
         end           
        end        
      end
      
      def add_map(map)
        map.each_layer do |layer_index, layer| 
          add_map_layer(layer, layer_index)
        end        
      end
       
       
    end
  end
end
