# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 
require 'chipmunk'
   

module Rogaru
  module Physics
    

    
    class Entity
      include CP
      attr_reader   :body
      attr_reader   :shape
      attr_accessor :static
      
      def initialize(x, y, wide, high, layer=0, mass=1.0)
        @wide   = wide.to_f - 1.0
        @high   = high.to_f - 1.0
        @high   = @wide if @high > @wide
        @hdif   = (high - @high  )
        @halfw  = wide / 2.0
        @halfh  = high / 2.0
        r1      = vec2(1      , 1       )
        r2      = vec2(1      , @high   )
        r3      = vec2(@wide  , @high   )
        r4      = vec2(@wide  , 1       )
        delta   = vec2(@halfw , @halfh  )
        verts   = [r1, r2, r3, r4]
        @static = (@mass == INFINITY)
        # @moment = @static ? INFINITY : moment_for_poly(mass, verts, delta)
        @moment = INFINITY # disable rotations 
        @body   = CP::Body.new(mass, @moment)
        @shape  = CP::Shape::Poly.new(@body, verts,  delta)
        @shape.u= 1.0 * mass unless @static         
        @body.p = vec2(x.to_f, y.to_f + @hdif)
        @shape.collision_type = :any
        if layer != 0
         # @shape.layers = 1 << layer
        else  
          # @shape.layers = 0xffffffff
        end  
        # Rectangular shape.
      end
      
      def layers=(l)
        @layers = l
      end
      
      def draw(screen, scroll_x=0, scroll_y=0)
        pos = @body.p - vec2(scroll_x, scroll_y)
        # screen position        
        c1  = [pos.x.to_i, pos.y.to_i]
        c2  = [pos.x.to_i + @wide, pos.y.to_i + @high]
        col = [255, 255, 255]
        col = [255,128, 128] if @static
        screen.draw_box(c1, c2, col)
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
    
    class Wall < Entity
      def initialize(x, y, wide, high, layer=0, mass=INFINITY)
        @x      = x
        @y      = y
        @wide   = wide.to_f
        @high   = high.to_f
        @halfw  = wide / 2.0
        @halfh  = high / 2.0
        r1      = vec2(0      , 0       )
        r2      = vec2(0      , @high   )
        r3      = vec2(@wide  , @high   )
        r4      = vec2(@wide  , 0       )
        delta   = vec2(@x + @wide, @y + @high + @halfh )
        verts   = [r1, r2, r3, r4]
        @static = true
        # @moment = @static ? INFINITY : moment_for_poly(mass, verts, delta)
        @moment = INFINITY # disable rotations 
        @body   = STATIC_BODY
        # @shape  = CP::Shape::Poly.new(@body, verts,  delta)
        @shape  = CP::Shape::Circle.new(@body, @halfw,  delta)
        @shape.collision_type = :wall
        if layer != 0
         # @shape.layers = 1 << layer
        else  
          # @shape.layers = 0xffffffff
        end  
        # Rectangular shape.
      end
      
      def draw(screen, scroll_x=0, scroll_y=0)
        px = @x - scroll_x
        py = @y - scroll_y
        # Screen position        
        c1  = [px + @halfw, py + @halfh]
        c2  = [px + @wide, py + @high]
        col = [255,128, 128]
        screen.draw_circle(c1, 20, col)
      end
      
      
    end
    
    class World
      
      attr_reader :space
      def initialize
        @space          = CP::Space.new()
        # @space.gravity  = VECTOR_ZERO
        @space.gravity  = CP::Vec2.new(0.0, 0.0)
        @substep        = 5
        @dt             = 1.0 / 60.0
        @space.damping  = 0.0
        @space.resize_static_hash(40.0, 50000 )
        @entities       = []        
        @statics        = []
        @space.add_collision_func(:any, :any) {
          |shape_1, shape_2|
#          p shape_1
#          p shape_2          
        }
      end
      
      def add(entity)        
        
        if(entity.static)                    
          @statics << entity
          @space.add_static_shape(entity.shape)
          @space.resize_static_hash(40.0, 50000 )
        else
          @entities << entity
          @space.add_body(entity.body)
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
        for ent in @statics do
          ent.draw(screen, scroll_x, scroll_y)
        end          
      end
     
      def add_map_layer(layer, layer_index)
        layer.each_tile do |tile, x, y|
         unless tile.nil? || tile.walk
           tile_ent = Wall.new(x, y, layer.tilewide, layer.tilehigh, 0)
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
