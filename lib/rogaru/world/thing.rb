module Rogaru
  class World
    class Thing 
      attr_accessor :index
      attr_accessor :body
      attr_accessor :shape
      
      def x 
        return self.body.p.x
      end
      
      def y
        return self.body.p.y
      end
       
      def vx
        return self.body.v.x
      end
      
      def vy
        return self.body.v.y
      end
      
      def x=(value)
        return self.body.p.x = value
      end
      
      def y=(value)
        return self.body.p.y = value
      end
       
      def vx=(value)
        return self.body.v.x = value
      end
      
      def vy=(value)
        return self.body.v.y = value
      end
      
      def ix
        return self.body.v.x
      end
      
      def iy
        return self.body.v.y
      end
      
      # Retuns the bounding box of the shape of this Thing
      def bounds
        return @shape.bb
      end
      
      
      attr_accessor :w
      attr_accessor :h
      attr_reader   :z
      attr_accessor :space
      
      def z=(znew)
        @z = znew
        if @shape
          @shape.layers = 1 << @z
        end
      end
      
      # Helper funcition to constuct vectors
      def vec(u,v)
        return CP::Vec2.new(u,v)
      end
      
      def camera_x
        return self.x.to_i        
      end
      
      def camera_y
        return self.y.to_i        
      end
      
      INFINITY =  10000000000
      
      def collision_type
        return nil unless @shape
        return @shape.collision_type
      end
      
      def collision_type=(t)
        return nil unless @shape
        return @shape.collision_type = t
      end
      
      
      def initialize(space, x, y, z, w, h, mass = INFINITY, impulse = INFINITY)
        @space        = space
        @body         = CP::Body.new(mass, impulse)
        points        = [vec(0, 0), vec(0, h), vec(w, h), vec(w, 0)]
        offset        = vec2(0,0) # vec(w/2, h/2)
        @shape        = CP::Shape::Poly.new(body, points, offset)
        @shape.thing  = self
        # Shapes's friction (u) should be set to 0 so only pushing is possible, 
        # not "rubbing"
        @shape.u      = 0.0
        # Correctly set shape's layer
        # @shape.layers = 1 << z
        self.x        = x
        self.y        = y
        self.z        = z        
        # @x = x ; @y = y ; @z = z
        @w = w ; @h = h
        @color = [255,255,0]
        @markedcolor = [255,0,0]
        @marked = false
        @index = -1
        @shape.collision_type = :thing
      end
      
      def mark!
        @marked=true
      end
      
      def apply_force(fx, fy)
        @body.apply_force(vec(fx, fy), vec(0, 0))
      end
      
      def apply_impulse(fx, fy)
        @body.apply_impulse(vec(fx, fy), vec(0, 0))
      end
      
      def render(screen, offset_x, offset_y)
        x = self.x + offset_x 
        y = self.y + offset_y
        # puts "#@x #@y #@w #@h #@color"
        color = @marked ? @markedcolor : @color 
        screen.put_rectangle(x, y, @w, @h, color)
        @marked=false          
      end
      
      # Things do nothing with dicplacement given.
      def give_displacement(dix, diy)
        return  
      end
      
      # Relative speed of motion towards other thing (if that thng can move)
      def relative_speed(other)
        othervx = (other.respond_to?(:vx) ? other.vx : 0)
        othervy = (other.respond_to?(:vy) ? other.vy : 0)
        vrelx   = othervx - @vx
        vrely   = othervy - @vy
        return vrelx, vrely
      end
      
      def nx
        @x
      end 
       
      def ny
        @y
      end   
      
      def nx=(x)
        @x = x
      end 
       
      def ny=(y)
        @y = y
      end   

      def can_be_pushed?(pusher)
        return false
      end
      
    end # class Thing
  end # class Space
end # module Rogaru
