module Rogaru
  
  # A sprite
  class Sprite
    # Defauilt time for animation steps
    DEFAULT_TIME = 1.0
    
    # A pose is a certain action or position that a Part of a sprite 
    # can perform. For example, the head can look down, up, etc,
    # and that in different directions. 
    # Each pose can consist of a single animation of several bitmaps
    class Pose
      
      
      # The kind of pose this is
      attr_reader :kind
      # The direction of this pose. Can be :north, :east, :south, :west, 
      # or :any if it is omnidirectional or applies in all directions.
      # If no pose for a given directory is available, it's :any counterpart 
      # will be used.
      attr_reader :direction
      
      # Creates a new direction
      def initialize(kind = :stand, direction = :any, frames = [], times = [])
        @kind       = kind.to_sym
        @direction  = direction.to_sym        
        @frames     = frames  # Frames of the animation.            
        @times      = times   # Animation with variable time frames.
        self.rewind()         # Rewind this pose's animation.                 
      end
      
      # Appends a frame to this pose, that will be shown during the given time 
      def add_frame(frame, time = DEFAULT_TIME)
        @frames   << frame
        @times    << time
      end  
      
      # Draws this pose on the given bitmap or screebn, at the given position
      def draw_at(screen, x, y)
        frame       = @frames.at(@active)
        screen.put_bitmap(frame, x, y)
      end
      
      # Rewinds this part's animation
      def rewind
        @active     = 0
        @remaining  = times[0] ||= DEFAULT_TIME
      end  
        
      # Advances this part's  animation by one frame
      def animate()
        @active    += 1
        @active     = 0 if @active >= @frames.size 
      end
      
      # Updates this part, taking variable time animation into consideration
      def update(time = DEFAULT_TIME)
        @remaining   -= time
        # If no more time remaining in this frame, move to the next one, and
        # We do not handle the case of frame skipping
        if @remaining < 0.0
          animate()
          @remaining  = @times[@active] || DEFAULT_TIME
        end
      end
      
    end
    
    # A Part is a part of a sprite that is layered and composed together 
    # with other parts to build a sprite. It can be animated, and 
    # change depending on the direction of the sprite, and
    # contain different Poses
    def Part
      # What kind of Part it is
      attr_reader :kind
      # x, y and z values of Parts are absolute, however you can use 
      #the rx ry rz accessors for relative updates 
      attr_reader   :x
      attr_reader   :y
      attr_reader   :z
      # Relative offset from the sprit'es position 
      # where this parts should be drawn
      attr_reader   :rx
      attr_reader   :ry
      attr_reader   :rz
      # the Sprite this part belongs to
      attr_reader   :sprite
      # Current active pose of this part. 
      # Can only be changes if it's sprite changes pose or direction.
      attr_reader   :pose
      # Is this part visible or not
      attr_accessor :visible
      
      # Updates the absolute X coorinates. Should be called whenever 
      # the sprite's X position, or this part's relative X position changes
      def update_x
        @x      = self.rx + @sprite.x        
      end
      
      # Updates the absolute Y coorinates. Should be called whenever 
      # the sprite's X position, or this part's relative Y position changes
      def update_y
        @y      = self.ry + @sprite.y
      end
      
      # Updates the absolute Z coorinates. Should be called whenever 
      # this part's relative Z position changes
      # Relative Z position is special in that it is independent of the 
      # sprite's Z position. This means that a part's Z only changes the drawing
      # order of the part within the sprite
      def update_z
        @z      = self.rz 
        # @sprite.part_z_changed(self, self.z)
      end      
      
      # Updates the x y and z coordinates of this part from it's parent
      # sprite 's parts
      def update_xyz
        update_x
        update_y
        update_z
      end
      
      # Creates a new part with the given kind, for the given sprite,
      # and with the given relative offsets 
      def initialize(sprite, kind = :body,  rx=0, ry=0, rz=0)
        @kind       = kind.to_sym
        @sprite     = sprite
        @rx         = rx
        @ry         = ry
        @rz         = rz
        @poses      = { :north => {}, :east => {}, :south => {}, :west=>{}, :any={}}
        @pose       = nil # Currently active pose
        @visible    = true
        update_xyz
      end
      
      # Adds a pose to this part, and retun it
      def add_pose(pose)
        @poses[pose.direction][pose.kind] = pose #store by direction and name
        @poses[:any][pose.kind]         ||= pose 
        # also store in :any if not set by something else (presumably more fitting)
        @pose = pose # Make newpose the currently active one
        return @pose 
      end
      
      # Makes a new pose, and adds it to this part
      def new_pose(kind, direction, frames = [], times = [])
        pose = Pose.new(kind, direction, frames, times)
        return self.add_pose(pose)
      end  
      
      # Should be called by @sprite whenever it's pose or direction changes
      # Note that he pose *must exist*, either in one of the 4 directions, 
      # or in the :any direction
      def update_pose
        @pose    = @poses[@sprite.direction][@sprite.pose]
        # If no direction-specific pose exists, use the :any pose
        unless @pose
          @pose  = @poses[:any][@sprite.pose]
        end
        # No niceties here, pose must be valid. Period.
        unless @pose
          raise "Sprite Part Error: No such pose for this part: #{@sprite.pose}"
        end  
        @pose.rewind # rewind the pose so it's ready to go
        return @pose
      end  
      
      # Sets the relative x of this part  
      def rx=(newrx)
        @rx = newrx
        update_x
      end
      
      # Sets the relative y of this part
      def ry=(newry)
        @ry = newry
        update_y
      end
      
      # Sets the relative z (layer) of this part
      def rz=(newrz)
        @rz = newrz
        update_z
      end
      
      # Should be called whenever time passes, to animate the part's active pose      
      def update_time(time = DEFAULT_TIME)
        raise "Sprite Part Error: @pose not set up correctly"
        @pose.update(time)
      end  
      
      # Draw this part's active pose to the screen
      def draw(screen)
        # Do nothing if not visible
        return unless @visible 
        # pose MUST have been set. No time here for checking or other niceties!        
        @pose.draw_at(screen, self.x, self.y)
      end
      
    end
    
    # List of sprites
    class List
      #Creates a new empty sprite list
      def initialize
        # All known sprites, ordered by name
        @sprites   = {}        
        # Sprites ordered in drawing layers
        @layers    = []
      end
      
      
      # Sorts the layer for this sprite by it's y values
      def sort_layer(sprite)
        @layers[sprite.z].sort_by {|s| s.y }
      end  
      
      # Puts a sprite into a layer
      def add_sprite_in_layer(sprite)
        @layers[sprite.z]   ||= []
        @layers[sprite.z]    << sprite
        sort_layer(sprite)
      end  
            
      # Adds a sprite to this list
      def add_sprite(sprite)
        @sprites[sprite.name] = sprite
        add_sprite_in_layer(sprite)
      end
      
      def new_sprite(name, x, y)          
      end
      
      # *Must* be called whenever the sprite's Z value changes,
      # to ensure the sprite ends up in the right layer
      def on_z_change(sprite, old_z, new_z)
        return false if new_z == old_z
        @layers[old_z].delete(sprite)
        add_sprite_in_layer(sprite)
        return true
      end
      
      # *Must* be called whenever the sprite's Y value changes,
      # to ensure the sprite is drawn correctly      
      def on_y_change(sprite, old_y, new_y)
        return false if new_y == old_y
        sort_layer(sprite)
        return true
      end
      
      
    end

    
    
    # Drawing locations of the sprite
    attr_accessor :x
    attr_reader   :y
    attr_reader   :z
    # The direction the sprite is currently facing 
    attr_reader   :direction 
    # The current pose the sprite is taking
    attr_accessor :pose
    attr_reader   :name
    # Is this sprite visible or not
    attr_accessor :visible

 
    # Creates a new sprite in the given list, with the given name and locations
    def initialize(list, name, x, y , z)
      @list           = list
      @x              = x
      @y              = y
      @z              = z
      @name           = name.to_sym
      # Parts ordered in drawing order (from back to front)      
      @drawing        = []
      # Parts in a hash by kind      
      @parts          = {}        
      @facing         = :south
      @pose           = :stand
    end  
    
    # draws this sprite at it's current location, on the given screen
    def draw(screen)
      # draw all parts of the sprite in order, from back to front
      for index in (0..@drawing.size) 
        part = @drawing[index]
        part.draw(screen)
      end
    end
    
    # Should be called whenever time passes, to animate the sprite      
    def update_time(time = DEFAULT_TIME)
      for part in @drawing do 
        part.update_time(time)
      end   
    end 
    
    # Should be called every game loop
    def update(time)
      update()
    end

    # Adds a part to this sprite, and return it
    def add_part(part)
      @parts[part.kind]  = part
      @drawing          << part
      @drawing.sort_by { |p| p.z }
      return part
    end
    
    # Adds a new part to this sprite and returns it
    def new_part(kind, rx, ry, rz)
      part = Part.new(self, kind, rx, ry, rz)
    end
    
    # Changes the Z value and the layer this sprite is in
    # The list this sprite is in must be set to do this
    def z=(new_z)
      old_z = self.z
      @z    = new_z
      @list.on_z_change(self, old_z, new_z)
    end  
    
    def y=(new_y)
      old_y = self.y
      @y    = new_y
      @list.on_y_change(self, old_y, new_y)
    end  
    
    
    
    
  end
end


































