# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 
module Rogaru
  module Physics
    INFINITY = 1.0 / 0.0
    class Vec2
      attr_accessor :x
      attr_accessor :y

      def initialize(x, y)
        @x = x ; @y = y
      end

      def to_splat
        return x, y
      end 
      
      def to_a
        return [x, y]
      end

      def +(other)
        return self.class.new(@x + other.x, @y + other.y)
      end

      def -(other)
        return self.class.new(@x - other.x, @y - other.y)
      end
    end
    

    class Collider
      def collide?(entity1, entity2)
      end
    end

    class Space


    end

    class Shape
      attr_accessor :layers
      attr_reader :p
      attr_reader :v
      def initialze(x, y)
        p       = vec2(x, y)
        v       = vec2(0, 0)
        layers  = 0
      end  
      
      class Circle
        attr_accessor :r
        def initialize(x, y, r)
          super(x, y) ;  @r = r
        end
      end

      class Box
        attr_accessor :w
        attr_accessor :h
        def initialize(x, y, w, h)
          super(x, y);  @w = w ; @h = h
        end
      end
    end

    class Body
      attr_accessor :mass
      attr_accessor :moment
      def initialize(mass, moment = 0.0 )
        @mass   = mass
        @moment = 0
      end
    end

    def vec2(x, y)
      return Vec2.new(x, y)
    end
    
    class Entity
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
        r1      = vec2(1      , 1     )
        r2      = vec2(1      , @high  )
        r3      = vec2(@wide  , @high  )
        r4      = vec2(@wide  , 1      )
        delta   = vec2( @halfw,  @halfh )
        verts   = [r1, r2, r3, r4]
        @static = (@mass == INFINITY)
        # @moment = @static ? INFINITY : moment_for_poly(mass, verts, delta)
        @moment = INFINITY # disable rotations 
        @body   = Body.new(mass, @moment)
        @shape  = Shape::Box.new(x, y, wide, high)        
        if layer != 0
         @shape.layers = 1 << layer
        else  
         # Don't change the shape's layer
        end  
        # Rectangular shape.
      end
      
      def camera_x
        return @shape.p.x.to_i
      end
      
      def camera_y
        return @shape.p.y.to_i
      end
      
      def layers=(l)
        @shape.layers = l
      end
      
      def draw(screen, scroll_x=0, scroll_y=0)
        pos = @body.p - vec2(scroll_x, scroll_y)
        # screen position        
        c1  = [pos.x.to_i, pos.y.to_i]
        c2  = [@wide, @high]
        col = [255, 255, 255]
        col = [255,128, 128] if @static
        screen.draw_disk(pos.x.to_i + @halfw, pos.y.to_i + @halfh, @halfw, col)
        screen.draw_box(*c1, *c2, col)
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
        delta   = vec2(@x + @wide, @y + @high )
        verts   = [r1, r2, r3, r4]
        @static = true
        # @moment = @static ? INFINITY : moment_for_poly(mass, verts, delta)
        @moment = INFINITY # disable rotations 
        @body   = STATIC_BODY
        # @shape  = CP::Shape::Poly.new(@body, verts,  delta)
        @shape  = CP::Shape::Circle.new(@body, @halfw,  delta)
        @shape.collision_type = :wall
        if layer != 0
         @shape.layers = 1 << layer
        else  
         # Don't change the shape's layer
        end          
      end
      
      def draw(screen, scroll_x=0, scroll_y=0)
        px = @x - scroll_x
        py = @y - scroll_y
        # Screen position        
        c1  = [px + @halfw, py + @halfh]
        c2  = [@wide, @high]
        col = [255,128, 128]
        screen.draw_ring(px + @halfw, py + @halfw, @halfw, col)
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
        @space.damping  = 0.1
        @space.resize_static_hash(32.0, 50000 )
        @entities       = []        
        @statics        = []
        @space.add_collision_func(:any, :any) {
          |shape_1, shape_2|
          puts "Colliding:" 
          p shape_1
          p shape_2          
        }
      end
      
      def add(entity)        
        
        if(entity.static)                    
          @statics << entity
          @space.add_static_shape(entity.shape)
          @space.resize_static_hash(32.0, 50000 )
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
        puts "Corner tile:"
        p layer.get_tile(0,0)
        p layer.tileset.tilewide
        layer.each_tile do |tile, x, y|
         if tile && (!tile.walk)
           tile_ent = Wall.new(x, y, layer.tileset.tilewide, layer.tileset.tilehigh, layer_index)
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
