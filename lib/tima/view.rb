# A class to model a view of a tile map, that is the currently visible 
# "window" of the tile map.
module Tima
  class View
    # Position vector 
    attr_reader :p
    # Size vector
    attr_reader :size
    
    # Initialize the camera around the x and the y points,
    # setting the size of the visible screen, and  
    # informing it about the maximum distance it may scroll (on the map)
    def initialize(x, y, w, h, xmax, ymax)
      @p        = vec(x, y)
      @size     = vec(w, h)
      @max      = vec(wmax, umax)
      @speed    = 10
      @goals    = []
      @tracking = []
      @lock     = nil
    end
    
    # X coordinate of camera view
    def x
      @p.x.to_i
    end
    
    def y
      @p.y.to_i
    end
    
    def w
      @size.x.to_i
    end
    
    def h
      @size.y.to_i
    end
    
    def w_half
      return (self.w / 2).to_i
    end
    
    def h_half
      return (self.w / 2).to_i
    end
    
    
    # Absolute x coordinate of the area center
    def cx
      ((self.p + self.size).x / 2).to_i
    end

    # Absolute y coordinate of the area center
    def cy
      ((self.p + self.size).y / 2).to_i
    end

    # Sets the center without doing any checks.
    def set_center(xnew, ynew)
      @p = vec(xnew - self.w_half, ynew - self.h_half)   
    end
      
    # Lefmost position for this camera's center
    def cx_min
      self.w_half
    end
      
    # Topmost position for this camera's center
    def cy_min
      return self.h_half
    end

    # Rightmost position for this camera's left side 's center
    def cx_max
      return @max.x - self.w_half
    end
    
    # Most "bottom"  position for this camera's top side's center
    def cy_max
      return @max.y - self.h_half
    end
    
    # Immediately centers the camera around this point.
    # Will clamp the result to acceptable values.
    def center(cx, cy)
      cx = cx_min if cx < cx_min
      cy = cy_min if cy < cy_min
      cx = cx_max if cx > cx_max
      cy = cy_max if cy > cy_max
      self.set_center(cx, cy)
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
    
    # Handles an update in case we're locked on to an object
    def lock_update(lock)
      center(lock.camera_x, lock.camera_y)
    end
    
    # Updates the camera's position based upon it's goal, lock on and tracking.
    def update
      lock_update(@lock) if @lock
    end
    
    
  end
end
