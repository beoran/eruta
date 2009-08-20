module Rogaru
  class Space
    class Actor < Mobile
      attr_accessor :color
      def initialize(space, x, y, z, w, h, vx = 0, vy = 0)
        super(space, x, y, z, w, h, vx, vy)
        @goal         = nil
        @other_goals  = []
        @memory       = []
        @max_speed    = 1
        self.can_push = true
        @color        = [0, 128, 255]
        @can_be_pushed= false
        @goal_max_dist= 300 
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
           
        deltax = @vx + ((goal.x - self.x).to_i / time)
        deltay = @vy + ((goal.y - self.y).to_i  / time)
        # Distance to go.
        deltax = @max_speed if @vx + deltax > @max_speed 
        deltay = @max_speed if @vy + deltay > @max_speed
        deltax = -@max_speed if @vx + deltax < -@max_speed 
        deltay = -@max_speed if @vy + deltay < -@max_speed         
         
        # Clamp speed to max speed.
        @vx    = deltax
        @vy    = deltay
      end

      def update(time)
        if (@goal)
          follow_simple(@goal, time)
        end 
      end


      def render(screen, offset_x, offset_y)
        x = @x + offset_x 
        y = @y + offset_y
        screen.fill_rectangle(x.to_i, y.to_i, @w, @h, @color)  
        screen.put_line(x.to_i, y.to_i, @ix, @iy, [255,255,255], false) 
      end




    end # class Thing
  end # class Space
end # module Rogaru