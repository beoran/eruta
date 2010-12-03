module Rogaru 
  class World
    # A zone is a non colliding area where a Thing can be inside (based on CP::BBox)
    class Zone < Thing
      attr_accessor :action
    
      def initialize(space, x, y, z, w, h, mass = INFINITY, impulse = INFINITY, &block)
        super(space, x, y, z, w, h, mass, impulse)
        self.collision_type = :zone
        @action             = &block
      end
      
      def act(who)
        return false unless @action
        @action.call(who, self)
      end
      
      def render(screen, offset_x, offset_y)
        x = self.x + offset_x 
        y = self.y + offset_y
        # puts "#@x #@y #@w #@h #@color"
        color = @marked ? @markedcolor : @color 
        screen.put_rectangle(x, y, @w, @h, color)
        @marked=false
      end
    end
  end
end 