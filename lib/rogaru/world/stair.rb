# A stair is a zone-like object that increases or decreases the Z level of a mobile thta passes over it.
# Currently, stairs can only alternate between two levels of the world 

module Rogaru 
  class World
    # A zone is a non colliding area where a Thing can be inside (based on CP::BBox)
    class Stair < Thing
      attr_reader :direction
    
      def initialize(space, x, y, z, w, h, mass = INFINITY, impulse = INFINITY, direction = :up)
        super(space, x, y, z, w, h, mass, impulse)
        self.collision_type = :stair
        @direction          = direction
        @color              = @direction == :up ?  Gari::Color.rgb(128,128,0) : Gari::Color.rgb(0,128,128)
      end
      
      def render(screen, offset_x, offset_y)
        x = self.x + offset_x 
        y = self.y + offset_y
        # puts "#@x #@y #@w #@h #@color"
        color = @marked ? @markedcolor : @color 
        screen.box(x, y, @w, @h, color)
        @marked=false
      end
    end
  end
end 