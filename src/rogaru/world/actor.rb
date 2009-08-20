module Rogaru
  class World
    class Actor < Mobile
      attr_accessor :color
      
      def initialize(space, x, y, z, w, h, mass = 1, impulse = INFINITY, vx = 0, vy = 0)
        super(space, x, y, z, w, h, mass, impulse, vx, vy)
        @goal         = nil
        @other_goals  = []
        @memory       = []
        @max_speed    = 1
        self.can_push = true
        @color        = [0, 128, 255]
        @can_be_pushed= false
        @goal_max_dist= 300 
        @shape.collision_type = :actor
      end
      
      def goal=(g)
       @goal = g
      end
    
      def distance_to_goal(goal)
         Math.sqrt((goal.x - self.x)**2 + (goal.y - self.y)**2) 
      end
      
      def halt()
        @vx = 0
        @vy = 0
        return self
      end

      def follow_simple(goal, time)
        distance = distance_to_goal(goal)
        # Give up following if too far or too close
        if distance > @goal_max_dist          
          return halt() 
        end
        if distance < Math.sqrt((@w)**2 + (@h)**2)
          return halt()
        end
           
        deltax = self.vx + ((goal.x - self.x).to_i / time)
        deltay = self.vy + ((goal.y - self.y).to_i  / time)
        # Distance to go.
        deltax = @max_speed if self.vx + deltax > @max_speed 
        deltay = @max_speed if self.vy + deltay > @max_speed
        deltax = -@max_speed if self.vx + deltax < -@max_speed 
        deltay = -@max_speed if self.vy + deltay < -@max_speed 
        # Clamp speed to max speed.
        # deltax = 0  if deltax.abs < deltay.abs 
        # deltay = 0  if deltay.abs < deltax.abs
        # Limit minimum speed to avoid jitter. The above doesn't work.
         
        
        self.vx    = deltax
        self.vy    = deltay
        self.apply_force(deltax, deltay)
      end

      def update(time)
        if (@goal)
          follow_simple(@goal, time)
        end 
      end


      def render(screen, offset_x, offset_y)
        x = self.x + offset_x 
        y = self.y + offset_y
        screen.fill_rectangle(x.to_i, y.to_i, @w, @h, @color)
        p self if self.ix != 0 || self.iy != 0
        screen.put_line(x.to_i, y.to_i, self.ix.to_i, self.iy.to_i, [255,255,255], false) 
      end




    end # class Thing
  end # class Space
end # module Rogaru