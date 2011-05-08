module Rogaru 
  class World
    class Wall < Thing
      def initialize(space, x, y, z, w, h, mass = INFINITY, impulse = INFINITY)
        @space        = space
        @body         = STATIC_BODY
        points        = [vec2(0, 0), vec2(0, h), vec2(w, h), vec2(w, 0)]
        delta         = vec2(x, y) # vec2(x + (w / 2), y + (h / 2))
        @shape        = CP::Shape::Poly.new(body, points, delta)
        # Correctly set shape's layer
        @shape.layers = 1 << z
        @shape.collision_type = :wall
        
        # self.x        = x
        # self.y        = y
        @x = x ; @y = y ; @z = z
        @w = w ; @h = h
        @color        = Gari::Color.rgb(0,255,127)
        @markedcolor  = Gari::Color.rgb(255,0,0)
        @marked       = false
        @index        = -1
      end
      
      def mark!
        @marked=true
      end
      
      def render(screen, offset_x, offset_y)
        x = @x + offset_x 
        y = @y + offset_y
        # puts "#@x #@y #@w #@h #@color"
        color = @marked ? @markedcolor : @color 
        screen.box(x, y, @w, @h, color)
        @marked = false
      end
    end
  end
end    