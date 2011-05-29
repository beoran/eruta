
# A class to model a camera that shows a view of a tile map
module Rogaru
  module Tilemap
    
    class Camera
      
      # A goal for the camera to scroll to
      class Goal
        attr_accessor :cx
        attr_accessor :cy
        attr_accessor :immediate
        attr_accessor :speed
        # Initialize the goal to the coordinates cx and cy,
        # and specifies if the goal has to bescrollled to instantly, 
        # or with a certain delay
        def initialize(cx, cy, immediate= false, speed = 10)
          @cx       = cx
          @cy       = cy
          @imediate = immediate
          @speed    = speed
        end
      end
      
      
      attr_reader :x
      attr_reader :y
      
      # Initialize the camera around the x and the y points,
      # setting the size of the visible screen, and  
      # informing it about the maximum distance it may scroll (on the map)
      def initialize(x, y, wide, high, xmax, ymax)
        @x        = x
        @y        = y
        @wide     = wide
        @high     = high
        @xmax     = xmax
        @ymax     = ymax
        @speed    = 10
        @goals    = []
        @tracking = []
        @lock     = nil
      end
      
      # Absolute x coordinate of the area center
      def cx
        (@x + (@wide/2)).to_i
      end

      # Absolute y coordinate of the area center
      def cy
        (@y + (@high/2)).to_i
      end

      # Set absolute x coordinate of the area center
      def cx=(other_cx)
        @x = other_cx - (@wide/2).to_i
      end

      # Set absolute y coordinate of the area center
      def cy=(other_cy)
        @y  = other_cy  - (@high/2).to_i 
      end
      
      # Sets the center without doing any checks.
      def set_center(x, y)
        self.cx = x
        self.cy = y
      end
      
      # Clamps Y center value
      def clamp_cy(y)
        return cy_min if y < cy_min
        return cy_max if y > cy_max
        return y
      end
      
      # Clamps X center value
      def clamp_cx(x)
        return cx_min if x < cx_min
        return cx_max if x > cx_max
        return x
      end
      
      # Clamps both x and y center value.
      def clamp_cx_cy(x, y)
        return clamp_cx(x), clamp_cy(y)
      end
      
      # Lefmost position for this camera's center
      def cx_min
        return (@wide / 2).to_i
      end
      
      # Topmost position for this camera's center
      def cy_min
        return (@high / 2).to_i
      end

      # Rightmost position for this camera's left side 's center
      def cx_max
        return @xmax - (@wide / 2).to_i
      end
      
      # Most "bottom"  position for this camera's top side's center
      def cy_max
        return @ymax - (@high / 2).to_i
      end
      
      # Immediately centers the camera around this point.
      # Will clamp the result to acceptable values.
      def center(cx, cy)
        new_x, new_y  = clamp_cx_cy(cx, cy)        
        self.cx       = new_x
        self.cy       = new_y 
      end
            
      # Locks camera to this object. A locked-on object MUST respond to the 
      # methods camera_x and camera_y. This is not checked for efficiency, 
      # so make sure that object does support these methods!
      # Set lock on to nil to relase camera lock.
      # A camera can be locked on to only one object. A lock on has priority 
      # over tracked objects, but not over scrolling goals.
      # Return self
      def lock_on(object=nil)
        @lock = object        
        return self
      end  
      
      # Adds a goal to this camera.
      # When +immediate+ is +false+ (default), the camera will scroll to 
      # center around the given point +cx+, +cy+ with the given +speed+ 
      # (default 10) 
      # When +immediate+ is +true+, the camera will center immediately on 
      # the given location.
      # In any case, the location will be clamped to what is possible for the 
      # camera to scroll to.
      # Goals have priority over the locked on object and the tracked objects.      
      # Goals will be visited in the order they have been added (FIFO)
      # Returns the goal. 
      def goal(cx, cy, immediate = false, speed = 10)
        cx , cy = clamp_cx_cy(cx, cy)
        g       = Goal.new(cx, cy, immediate, speed)
        @goals << g
        return g
      end
      
      # Removes a goal from the goals
      # Returns self
      def goal_remove(goal)
        @tracking.delete(goal) 
        return self
      end

      # Adds an object to track to this camera. The camera will do it's best to 
      # show all tracked objects, and it will scroll to do so, but only if one 
      # or more of the objects nears the edge of the screen. 
      # If the objects are "pulling" in opposite directions, then the screen 
      # will not scroll.
      # In case the object is in the edge of the camera view, it will have it's 
      # camera_edge method called on every update of the camera.
      # In case the object is not visible to this camera, it's 
      # camera_insidible method will be called on every update of camera.
      # A trackable object MUST respond to the methods camera_x,
      # camera_y (not checked!) and optionally MAY to camera_edge and 
      # camera_hidden (checked with respond_to?).
      # Trackable objects have a lower priority than the locked on object,
      # or than the goals.
      # Tracked objects will be given tracking priority in the order they have 
      # been added (FIFO)
      # XXX: you can add a tracked object, but it does not work yet.
      def track(object)
        @tracking << object
      end
      
      # Stops tracking an object.
      # Returns self.
      def stop_tracking(object)
        @tracking.delete(object) 
        return self
      end

      # Returns true if the goal has been reached, false if not
      def goal_reached?(goal)
        return self.cx == goal.x && self.cy == goal.y        
      end
      
      # Handles an update in case we're trying to reach a goal
      def goal_update(goal)
        # If we reached the goal, remove it.
        if goal_reached?(goal)
          goal_remove(goal)
          return true
        end
        # If the goal is immediate, go there right away, and remove it.
        if goal.immediate
          center(goal.x, goal.y)
          goal_remove(goal)
          return true
        end
        
        # If we get here, we have a scroll-to goal. Scroll to it.
        self_cx= self.cx
        self_cy= self.cy
        deltax = goal.x - self_cx
        deltay = goal.y - self_cy
        # Distance to go.
        deltax = goal.speed if deltax > goal.speed 
        deltay = goal.speed if deltay > goal.speed         
        # Clamp distance to speed.
        center(self_cx + deltax, self_cy + deltay)
        # And move the center a bit closer to the goal.
        return false
      end
      
      # Handles an update in case we're locked on to an object
      def lock_update(lock)
        center(lock.camera_x, lock.camera_y)
      end
      
      # Handles update of the camera for tracked objects.
      # XXX: not implemented yet.
      def track_update
        
      end
      
      # Updates the camera's position based upon it's goal, lock on and tracking . 
      def update
        if @goals.size > 0
          goal = @goals.first
          goal_update(goal)
        elsif @lock
          lock_update(@lock)
        elsif @tracking.size > 0
          track_update()
        end        
      end
      
      
    end
    
  end
end
