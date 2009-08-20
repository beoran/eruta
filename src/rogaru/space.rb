
module Rogaru

  # A space simulates a "physical" world in 2.5 dimensions.  
  class Space
    
    # A Body is a rectangular object inside the space that has a 
    # certain size
    class Body
      attr_accessor :index 
      attr_accessor :w
      attr_accessor :h
      
      def initialize(space, w, h)
        @space  = space
        @w      = w 
        @h      = h
        @color  = [255, 0, 0]
        @index  = -1
      end
      
      # Determines the amount of intersection on one axis
      # A positive result means an intersection by that much, 
      # zero or a negative result one means no intersection
      def self.axis_intersect(pos1, dim1, pos2, dim2)
        if (pos2 > pos1)
          return pos1 - pos2 + dim1
        else
          return pos2 - pos1 + dim2
        end
      end

      # Determines the intersection betwen this body and another one if
      # they would be positioned at the given positions.
      #  
      def intersect_at(my_x, my_y, body, body_x, body_y)
        intersect_x = Body.axis_intersect(my_x, self.w, body_x, body.w)
        intersect_y = Body.axis_intersect(my_y, self.h, body_y, body.h)
        return intersect_x, intersect_y
      end
    
    # sees if two objects collide if they are at the given position
    def detect_collision_at?(first, second, fx, fy, sx, sy)
      return false if first.index == second.index 
      # Don't collide with self.
      return false if first.z != second.z
      # Don't collide if on another level.
      xcol = axis_collide?(fx, first.w, sx, second.w)
        
      end
      
    end
    
    autoload :Actor    , 'rogaru/space/actor'
    autoload :Collision, 'rogaru/space/collision'
    autoload :Mobile   , 'rogaru/space/mobile'
    autoload :Thing    , 'rogaru/space/thing'

    # A sort of grid based "spacial lookup" or "spatial hash", that tracks which 
    # objects are close to each other. Currectly only useful for static 
    # objects, as there is no way to remove/move any objects individually apart
    # from clearing the structure as a whole. 
    # The spattial hash automatically accomodates spaces of any sizes, and does 
    # not need to know the exact dimensions of the space.
    # the grid cells can be of any size, even smaller than the size of the 
    # objects. However, however, as a result of this, an object can be present 
    # in more than one grid cell if it's size is bigger than that of the grid, 
    # or if it is positioned just at the edge of the grid cells.     
    class Lookup
      # Width of a grid cell in the lookup
      attr_reader :xgridsize
      # Height of a grid cell in the lookup 
      attr_reader :ygridsize

      # Sets up the spatial lookup with the given grid sizes  
      # The smaller the grid size, the more memory the grid will use. 
      # Speed depends on object sizes. A smaller grid size is usually faster. 
      # The default of 128 is nice for a world that uses 32x32 tiles. 
      def initialize(xgridsize = 128, ygridsize = 128)
          @xgridsize  = xgridsize
          @ygridsize  = ygridsize
          @lookup     = {}           # Y/X matrix 
      end

      def inspect
        return @lookup.inspect
      end

      # Empties this lookup.
      def clear
        for row in @lookup do
          for cell in row do
            cell.clear
          end
        end
      end
      
      # Converts Space xy coordinates to this lookup's grid coordinates.
      # note that for techical reasons, negative values of x and y will lead 
      # to grid coordinates of 0 
      def xy_to_grid(x, y)
        gridx = (x / @xgridsize).to_i
        gridy = (y / @ygridsize).to_i
        gridx = 0 if gridx < 0
        gridy = 0 if gridy < 0
        return gridx, gridy 
      end

      # Returns the coordinates of the collision grid points.      
      def overlap(thing)
        return grid_overlap(thing.x, thing.y, thing.w, thing.h)         
      end  

      # Returns the indexes of the grid cells that an object at Space 
      # coordinates (x, y) any with a bounding box size of w and h will "cover"       
      def grid_overlap(x, y, w, h)
        result = []
        xstart, ystart =  xy_to_grid(x, y)
        xstop , ystop  =  xy_to_grid(x + w, y + w)
        # The four grid coordinates of the corners
        # All the grid coordinates must be between (inclusive) these 
        for ydex in (ystart..ystop) do 
          for xdex in (xstart..xstop) do
            gridx, gridy = xdex, ydex
            result << [gridx, gridy]
          end
        end  
        return result
      end

      # Stores a Thing in this lookup. Thing must respond to .x .y .w and .h 
      def store(thing)
        self.store_object(thing.x, thing.y, thing.w, thing.h, thing)  
      end
      
      # Looks up any objects that may interact with the thing.
      # Thing must respond to .x .y .w and .h
      def lookup_rectangle_thing(thing)
        return lookup_rectangle(thing.x, thing.y, thing.w, thing.h)
      end
      
      # Stores an object with the given location and size into this lookup
      def store_object(x, y, object_w, object_h, object)
        indexes = grid_overlap(x, y, object_w, object_h)
        for index in indexes do
          xdex, ydex = *index
          @lookup[ydex]       ||= {}
          @lookup[ydex][xdex] ||= []
          @lookup[ydex][xdex] << object
        end
      end

      # Gets all objects in this lookup that may overlap with the rectangle 
      # defined by (x, y, w, h). Returns an array with the objects, or an empty 
      # array if no such objects could be found. 
      def lookup_rectangle(x, y, w, h)
        result = []  
        indexes = grid_overlap(x, y, w, h)
        for index in indexes do
          xdex, ydex = *index
          if @lookup[ydex] && @lookup[ydex][xdex]            
            result +=@lookup[ydex][xdex]
          end   
        end
        return result
      end
    end
 
       
    

    attr_accessor :things
    attr_accessor :mobiles
    attr_accessor :iterations
    
    def initialize()
      @things     = []
      @mobiles    = []
      @actors     = []
      @all        = []
      @iterations = 1
      @collisions = []
      @lookup     = Lookup.new()
      @grid       = {}
      @grid_wide  = 32
      @grid_high  = 32
      @marked     = [] 
    end
    
    # Adds grid information 
    def add_grid(x, y, z, info = false)
      y               = y.to_i
      x               = x.to_i
      @grid[z]        = [] if !@grid[z]
      @grid[z][y]     = [] if !@grid[z][y]
      @grid[z][y][x]  = info
    end 
    
    # Adds immobile thing to the space 
    def add_thing(thing)
      @things << thing
      @all    << thing
      @lookup.store(thing)
      thing.index = @all.size - 1
      return thing
    end

    # Adds mobile to the space.
    def add_mobile(mobile)
      @mobiles << mobile
      @all     << mobile
      mobile.index = @all.size - 1
      return mobile
    end

    # Adds actors to the space.
    def add_actor(actor)
      @mobiles << actor
      @all     << actor
      @actors  << actor
      actor.index = @all.size - 1
      return actor
    end
    
    
    # Adds new immobile thing to the space
    def new_thing(x, y, z, w, h)
      return add_thing(Thing.new(self, x, y, z, w, h))  
    end
    
    # Adds new mobile to the space (that is, a thing that may be moved)  
    def new_mobile(x, y, z, w, h, vx = 0, vy =0)
      return add_mobile(Mobile.new(self, x, y, z, w, h, vx, vy)) 
    end

    # Adds new actor to the space (that is, a thing that may move autonomously 
    # or that is controlled.)
    def new_actor(x, y, z, w, h, vx = 0, vy =0)
      return add_actor(Actor.new(self, x, y, z, w, h, vx, vy)) 
    end
    
    
    def axis_collide?(p1, s1, p2, s2)
      if    (p1 < p2)
        return (p1 + s1) >= p2
      else
        return p1 <= p2 + s2
      end
    end
    
    # sees if two objects collide if they are at the given position
    def detect_collision_at?(first, second, fx, fy, sx, sy)
      return false if first.index == second.index 
      # Don't collide with self.
      return false if first.z != second.z
      # Don't collide if on another level.
      xcol = axis_collide?(fx, first.w, sx, second.w)
      return false unless xcol
      ycol = axis_collide?(fy, first.h, sy, second.h)  
      return ycol      
    end
    
    # sees if two objects collide
    def detect_collision?(first, second)
      return false if first.index == second.index 
      # Don't collide with self.
      return false if first.z != second.z
      # Don't collide if on another level.
      xcol = axis_collide?(first.nx, first.w, second.nx, second.w)
      return false unless xcol
      ycol = axis_collide?(first.ny, first.h, second.ny, second.h)
      return ycol      
    end
    
    # returns the pushback, or 0 if no overlap 
    def self.minimal_pushback(p1, s1, p2, s2)
      push_back = p2 - (p1 + s1) - 1
      push_on   = (p2 + s2) - p1 + 1
      return  [push_back, push_on].sort_by{ |x| x.abs }.first
    end
    
    # Resolves collision with a solid, non-movable object.    
    def resolve_solid(mobile, other)
        xpush = Space.minimal_pushback(mobile.nx, mobile.w, other.nx, other.w)
        ypush = Space.minimal_pushback(mobile.ny, mobile.h, other.ny, other.h)
        # finds minimals push-back in both directions.
        # puts "#{xpush}, #{ypush}"
        if xpush.abs < ypush.abs and xpush != 0.0
          mobile.nx += xpush
          mobile.vx  = 0 # must reset speed also
        elsif ypush.abs < xpush.abs and ypush != 0.0
          mobile.ny += ypush
          mobile.vy  = 0 # must reset speed also
        else 
          mobile.nx += xpush
          mobile.vx  = 0 # must reset speed also
           if detect_collision?(mobile, other) 
            mobile.ny += xpush
            mobile.vy  = 0 # must reset speed also
           end
        end
    end
    
    # Resolves collision with a pushable object.
    def resolve_pushable(mobile, other)
        xpush = Space.minimal_pushback(mobile.nx, mobile.w, other.nx, other.w)
        ypush = Space.minimal_pushback(mobile.ny, mobile.h, other.ny, other.h)
        # finds minimals push-back in both directions.
        # puts "#{xpush}, #{ypush}"
        if xpush.abs < ypush.abs and xpush != 0.0
          mobile.nx += xpush
        elsif ypush.abs < xpush.abs and ypush != 0.0
          mobile.ny += ypush
        else 
          mobile.nx += xpush
           if detect_collision?(mobile, other)
            mobile.ny += xpush
           end
        end     
    end
    
    
    def resolve_collision(mobile)
      # xpush = Space.minimal_pushback(first.nx, first.w, second.nx, second.w)
      # ypush = Space.minimal_pushback(first.ny, first.h, second.ny, second.h)
      # puts mobile.collide.map { |c| c.index }
      xpushes = []
      ypushes = []
      # resolve collision with all colliding other things and mobiles.
      mobile.collide.each do  |other|
        next unless detect_collision?(mobile, other) 
        # Skip those that don't collide anymore.
        if (mobile.can_push && other.can_be_pushed?(mobile))
          resolve_pushable(mobile, other)  
          # other.vx = mobile.vx
          # other.vy = mobile.vy
          other.give_impulse(mobile.vx, mobile.vy)    
        else  
          resolve_solid(mobile, other)  
        end        
        # push the mobile back in the shortest distance possible, 
        # preferrring the direction of smallest nonzero displacement 
      end        
    end
    
    # Returns a list of all things and mobiles this mobile /may/ possibly 
    # collide with, that is, those objects that are theoretically in range for 
    # collision, if it is positioned at (mx, my)     
    def mobile_may_hit_list(mobile, mx, my)
      may_hit_thing = @lookup.lookup_rectangle(mx , my, mobile.w, mobile.h)
      may_hit       = may_hit_thing + @mobiles
      return may_hit
    end

    # Returns a list of all other thingst his mobile will collide with
    # if it is moved to mx my. Returns set of collided objects, or 
    # nil if no such objects are there.
    def mobile_collide_list_at(mobile, mx, my)
      result  = []
      mobile.collide_reset 
      may_hit = mobile_may_hit_list(mobile, mx, my)     
      for other in may_hit do
        if detect_collision_at?(mobile, other, mx, my, other.nx, other.ny)        
          result         << other
        end
      end
      return result if result.size > 0
      return nil
    end

    # Returns a list of all other thingst his mobile will collide with
    # if it is moved to self.nx self.ny
    # also set's the mobile's collide list
    # returns true if it collided with anything, false if it didn't.
    def mobile_collide_list(mobile)
      result        = false
      mobile.collide_reset
      may_hit       = mobile_may_hit_list(mobile, mobile.x, mobile.y)
      for other in may_hit do
        if detect_collision?(mobile, other)
          mobile.collide  << other
          result = true
        end
      end
      return result
    end
    
    # Resolves collision with a solid, non-movable object.
    def resolve_solid_new(mobile, other, xover, yover)
        xpush = xover
        ypush = yover
        if xpush.abs < ypush.abs # and xpush != 0
          if mobile.ix > 0
            mobile.ix -= xpush
          else   
            mobile.ix += xpush
          end  
          mobile.nx = mobile.x + mobile.ix
        else 
        # if ypush.abs < xpush.abs # and ypush != 0
          if mobile.iy > 0
            mobile.iy -= ypush
          else
            mobile.iy += ypush
          end  
          mobile.ny = mobile.y + mobile.iy
=begin          
        else 
          mobile.ix += xpush
          mobile.nx = mobile.x + mobile.ix
          intersect_x, intersect_y = mobile.intersect_at(mobile.nx, mobile.ny, other, other.x, other.y)
          if (intersect_x > 0) && (intersect_y > 0)
            if mobile.iy > 0
              mobile.iy -= ypush
            else
              mobile.iy += ypush
            end
            mobile.ny = mobile.y + mobile.iy
          end  
=end          
        end
    end
    
    
    # looks up the tiles in the given rectangle and returns the tile coordinates
    # of those that do collide in an array.
    def grid_lookup_rectangle(left, top, wide, high, z)
      gleft   = (left / @grid_wide).to_i
      right   = left + wide
      gright  = (right / @grid_wide).to_i
      gtop    = (top / @grid_high).to_i
      bottom  = top + high
      gbottom = (bottom / @grid_high).to_i
      result  = []
      
      for gy in gtop..gbottom
        for gx in gleft..gright
          solid = self.get_grid(gx, gy, z)  
          next unless solid
          result << [gx, gy, @grid_wide, @grid_high] 
        end
      end  
      return result    
    end
    
    GRID_SIZE = 32
    
    # Let the things limit the mobile's motion
    def collide_mobiles_things
      
      for mobile in @mobiles do
        mhw       = mobile.w 
        mhh       = mobile.h 
        hit_thing = grid_lookup_rectangle(mobile.nx, mobile.ny, mobile.w, mobile.h, mobile.z)
        # may_hit_thing = @lookup.lookup_rectangle(mobile.nx - mhw, mobile.ny - mhh, mhw * 2, mhh * 2)
        @marked  += hit_thing 
        for thing in hit_thing do 
          gx = thing[0]
          gy = thing[1]
          tx = gx * GRID_SIZE
          ty = gy * GRID_SIZE
          if mobile.vx > 0  && (mobile.nx + mobile.w) > tx  # coming from the left, overlapping on the left
            mobile.nx = tx - mobile.w
            mobile.vx = 0
          elsif mobile.vx < 0 && mobile.nx < (tx + GRID_SIZE)   # coming from the right, overlapping on the right
            mobile.nx = (tx + GRID_SIZE)
            mobile.vx = 0
          end          
          if mobile.vy > 0  && (mobile.ny + mobile.h) > ty  # coming from the top, overlapping on the top
            mobile.ny = ty - mobile.h
            mobile.vy = 0 
          elsif mobile.vy < 0 && mobile.ny < (ty + GRID_SIZE)   # coming from the bottom, overlapping on the bottom
            mobile.ny = (ty + GRID_SIZE)
            mobile.vy = 0
          end

        
          # intersect_x, intersect_y = mobile.intersect_at(mobile.nx, mobile.ny, thing, thing.x, thing.y)
          # next unless (intersect_x > 0) && (intersect_y > 0)
          
          # thing.mark!
          # resolve_solid_new(mobile, thing, intersect_x, intersect_y)
=begin           
          next
          
          iangle  = Math.atan2(mobile.iy, mobile.ix)
          nangle  = Math.atan2(intersect_y, intersect_x)
          angle   = iangle - nangle
          # angle   = nangle - iangle if angle < 0
          # angle   = Math::PI - angle if angle > Math::PI
          normal_force = Math.sqrt((intersect_y**2 + intersect_x**2)) * Math.cos(angle)
          # normal_x     = -Math.cos(iangle) * intersect_x  
          normal_x     = normal_force * Math.cos(iangle) 
          # normal_y     = -Math.sin(iangle) * intersect_y  
          normal_y     = normal_force * Math.sin(iangle) 
          if  mobile.ix < 0
            mobile.ix  = mobile.ix + normal_x            
          else  
            mobile.ix  = mobile.ix - normal_x
          end
          if  mobile.iy < 0
            mobile.iy  = mobile.iy + normal_y            
          else  
            mobile.iy  = mobile.iy - normal_y
          end
          mobile.nx    = mobile.x + mobile.ix
          mobile.ny    = mobile.y + mobile.iy
          
          # for collision resolutiion, the pushout and the impulse (future motion need to be combined
          # however, this is not a simple vector sum!. The push-out force is in the same direction as the 
          # impulse vector, but of opposed sign.  
          
         
          if mobile.ix > 0
            mobile.ix = mobile.ix - intersect_x
          else 
            mobile.ix = mobile.ix + intersect_x
          end
          mobile.nx = mobile.x + mobile.ix
          # Do we have to check intersection again, to prevent being moved diagonally when not needed? 
          # intersect_x, intersect_y = mobile.intersect_at(mobile.nx, mobile.ny, thing, thing.x, thing.y)
          # next unless (intersect_x > 0) && (intersect_y > 0)
          if mobile.iy > 0
            mobile.iy = mobile.iy - intersect_y
          else 
            mobile.iy = mobile.iy + intersect_y
          end
          mobile.ny = mobile.y + mobile.iy
          
          
          
          if intersect_x > 0
            if mobile.ix > 0
              mobile.ix = mobile.ix - intersect_x
            else 
              mobile.ix = mobile.ix + intersect_x
            end    
            mobile.nx = mobile.x + mobile.ix  
          end
          if intersect_y > 0
            mobile.iy = mobile.iy - intersect_y 
            mobile.ny = mobile.y + mobile.iy
          end
=end          
        end
      end  
      
    end
    
    # Updates the Space (passing a given amount of time in that space)
    def update(time=1.0)
      @actors.each { |actor| actor.update(time) } 
      @collisions = {}  
      @collidelist= []
      # Predict positions if there was no collision active
      @mobiles.each { |m| m.predict_position(time)  }
      collide_mobiles_things
      # First, let mobiles resolve collision with the things (the grid) 
      # Then let the collide with each other in a depth-first search       
      @mobiles.each { |m| m.finalize_movement() }
    end


    
    # Updates the Space (passing a given amount of time in that space)
    def update_old(time=1.0)
      @actors.each { |actor| actor.update(time) } 
      @collisions = {}  
      @collidelist= []          
      @mobiles.each { |m| m.predict_position(time)  }
      allsize = @all.size
      mobsize = @mobiles.size
      # Wide collision check 
      for mobile in @mobiles do
        hit = mobile_collide_list(mobile)
        if hit
            @collidelist     << mobile
        end
      end
      # Resolve collisions 
      for mobile in @collidelist do
        resolve_collision(mobile)
      end        
      @mobiles.each { |m| m.finalize_movement() }
    end

    # Draws on-screen diagnostics like bounding boxes and 
    # object indexes of objects in this Space on the screen. 
    def draw(screen, scroll_x, scroll_y, font = nil)  
      @drawindex  = false
      @drawwide   = false
      offset_x    = -scroll_x
      offset_y    = -scroll_y
      ttcol       = [250, 250, 250]
=begin      
      @things.each  do 
          |t| t.render(screen, offset_x, offset_y)
          if font and @drawindex
            font.draw(screen, t.index.to_s, t.x + 5 + offset_x, t.y + 5 + offset_y, ttcol)
          end           
      end
=end     
      # Show tiles hit for debugging 
      @marked.each do | m |
        x = m[0] * @grid_wide + offset_x 
        y = m[1] * @grid_high + offset_y
        screen.put_rectangle(x, y, @grid_wide, @grid_high, [255,0,0])
      end
      @marked = []
      
      @mobiles.each do |m| 
          m.render(screen, offset_x, offset_y) 
          if font and @drawindex
            font.draw(screen, m.index.to_s, m.x + 5 + offset_x, m.y + 5 + offset_y, ttcol)
          end           
          if m.index == 0 and @drawwide
            points = @lookup.overlap(m)          
            for point in points do
              x, y = * point
              x   = x * @lookup.xgridsize
              y   = y * @lookup.ygridsize
              screen.draw_box(x + offset_x, y + offset_y, @lookup.xgridsize, @lookup.ygridsize, [255,127,0]) 
            end
          end          
      end
    end
    
    
    # Get element from grid with grid coordinates
    def get_grid(gx, gy, gz = 0)
      plane     = @grid[gz]
      return nil unless plane
      row       = plane[gy]
      return nil unless row
      return row[gx]
    end
    
    # Get element from grid, using normal world coordinates. 
    # Returns nil or false if not found or nothing "physical" there.
    def get_grid_xy(xx, yy, zz = 0)
      gx, gy    = to_grid(xx, yy)
      return get_grid(gx, gy, zz)
    end
    
    # Map coordinates to grid coordinates
    def to_grid(xx, yy)
      return xx / @grid_wide, yy / @grid_high
    end
    
    # Adds one layer from a tile map to the world simulation
    def add_map_layer(layer, layer_index)
      layer.each_tile do |tile, x, y|
       if tile && (!tile.walk)
         w      = layer.tileset.tilewide - 2
         h      = layer.tileset.tilehigh - 2
         self.new_thing(x + 1, y + 1, layer_index, w, h)
         gx, gy = to_grid(x, y)
         self.add_grid(gx, gy, layer_index, :solid)
       end
      end
    end

    # Adds a tile map to the world simulation
    def add_map(map)
      map.each_layer do |layer_index, layer| 
        p "layer index:", layer_index
        add_map_layer(layer, layer_index)
      end        
    end
  end
end

if $0 == __FILE__
Space = Rogaru::Space
spl   = Space::Lookup.new(32*1000, 32*1000)

 

end



