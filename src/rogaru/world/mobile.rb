module Rogaru
  class World
    # A mobile is a thing in a Space that can be moved or that can move.
    class Mobile < Thing
      attr_accessor :vx
      attr_accessor :vy
      # New position of the object
      attr_accessor :nx
      attr_accessor :ny
      # Displacement (impulse) over the next step, wanted and actual
      attr_accessor :want_ix
      attr_accessor :want_iy
      attr_accessor :ix
      attr_accessor :iy
      
      # pushability flags 
      attr_accessor :can_push
      attr_reader   :collide
      
      def initialize(space, x, y, z, w, h, mass = 1, impulse = INFINITY, vx = 0, vy = 0)
        super(space, x, y, z, w, h, mass, impulse)
        @vx       = vx ; @vy = vy 
        @color    = [0, 255, 255]
        @index    = -1 
        @nx       = @y ; @ny = @x 
        # motion this object will make in the next step
        # and the new position it will aquire
        @want_ix  = @ix  = 0
        @want_iy  = @iy  = 0  
        @impulsex = 0
        @impulsey = 0
        # Displacement is not reset, but passed on
        @collide  = []  
        # Things this mobile is colliding with. 
        @can_push = false
        @shape.collision_type = :mobile
      end
      
      def collide_reset
        @collide = []  
      end
      
      def update(time=1.0)
      end
      
      
      
      # can this object be pushed by the pusher?
      def can_be_pushed?(pusher)
        return false if @collide.size != 1
        # Make really sure that we also won't collide there in the next step.
        # colx    = (pusher.vx == 0.0 ? 0.0 : pusher.vx > 0.0 ? 1 : -1)  
        # coly    = (pusher.vy == 0.0 ? 0.0 : pusher.vy > 0.0 ? 1 : -1)
        collist = @space.mobile_collide_list_at(self, pusher.vx + @nx, pusher.vy + ny)
        # XXX: May need a time factor in here.
        # collist = @space.mobile_collide_list_at(self, colx + @nx, coly + @ny)
        if collist && collist.size > 0
          return false 
        end          
        # Cannot push if it's collinding with other stuff, or will be 
        # colliding with other stuff in it's next location.
        # This also prevents pushing one with the other which 
        # may have been nice in some cases, but which is probably not needed.
        return true
      end

      # This gives the mobile an "impulse", that is, a short movement
      # that will be calculated in on the next world update.
      def give_impulse(x, y)
        @impulsex = x
        @impulsey = y
      end
      
      # Calculate what the next position of this object would be if it doesn't 
      # collide with anything. Also sets @ix, @iy, @want_ix, @want_iy.
      def predict_position(time = 1.0)
        @ix   = @want_ix = @vx * time + @impulsex
        @iy   = @want_iy = @vy * time + @impulsey
        @nx   = @x + @want_ix
        @ny   = @y + @want_iy
        @impulsex = 0
        @impulsey = 0
      end
      
      def render(screen, offset_x, offset_y)
        x = self.x + offset_x 
        y = self.y + offset_y
        screen.fill_rectangle(x.to_i, y.to_i, @w, @h, @color)
      end
      
      def finalize_movement()
        @x = @nx
        @y = @ny
      end

    end # class Mobile
  end # class Space
end # module Rogaru