module Bump

  # A Lookup is a grid based "spacial lookup" or "spatial hash", 
  # that tracks which objects are close to each other. 
  # The spatial hash automatically accomodates spaces of any sizes, and does 
  # not need to know the exact dimensions of the space.
  # The grid cells can be of any size, even smaller than the size of the 
  # objects. However, as a result of this, an object can be present 
  # in more than one grid cell if it's size is bigger than that of the grid, 
  # or if it is positioned just at the edge of the grid cells.

  class Lookup
    # Width and height of a grid cell in the lookup. Grid cells are square for
    # simplicity's sake
    attr_reader :gridsize

    # Sets up the spatial lookup with the given grid size.  
    # The smaller the grid size, the more memory the grid will use. 
    # Speed depends on object sizes. A smaller grid size is usually faster. 
    # The default of 128 is nice for a world that uses 32x32 tiles. 
    def initialize(gridsize = 128)
        @gridsize  = gridsize
        @xgrid     = {}           # Y/X matrix 
        @ygrid     = {}
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
    
    # Scales a value to this lookup's grid dimensions
    def to_grid(value)
      return (value / @gridsize).to_i
    end
    
    # Converts Space xy coordinates to this lookup's grid coordinates.
    def xy_to_grid(x, y)
      return to_grid(x), to_grid(y)
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

    # Yields the indexes of the grid cells that an object at Space 
    # coordinates (x, y) any with a bounding box size of w and h will "cover"  
    def grid_overlap_yield(x, y, w, h)
      result = []
      xstart, ystart =  xy_to_grid(x, y)
      xstop , ystop  =  xy_to_grid(x + w, y + w)
      # The four grid coordinates of the corners
      # All the grid coordinates must be between (inclusive) these 
      for ydex in (ystart..ystop) do 
        for xdex in (xstart..xstop) do
          gridx, gridy = xdex, ydex
          yield gridx, gridy
        end
      end  
      return result
    end

    # Stores an object in this lookup. Object must respond to .x .y .w and .h 
    def <<(object)
      grid_overlap_yield(object.x, object.y, object.w, object.h) do 
        |xdex, ydex|
        @ygrid[ydex]       ||= [] 
        @xgrid[xdex]       ||= []
        @ygrid[ydex]        << object
        @xgrid[xdex]        << object
      end
    end
    
    # Removes an object from the lookup. Useful when the object moves.
    def remove(object)
      grid_overlap_yield(object.x, object.y, object.w, object.h) do 
        |xdex, ydex|
        @ygrid[ydex].delete(object) if @ygrid[ydex]
        @xgrid[xdex].delete(object) if @xgrid[xdex]
      end
    end
    
    # Looks up any objects that may interact with the thing.
    # Thing must respond to .x .y .w and .h
    def [](thing)
      return lookup_rectangle(thing.x, thing.y, thing.w, thing.h)
    end

    # Gets all objects in this lookup that may overlap with the rectangle 
    # defined by (x, y, w, h). Returns an array with the objects, or an empty 
    # array if no such objects could be found. 
    def lookup_rectangle(x, y, w, h)
      result = []  
      grid_overlap_yield(x, y, w, h) do |xdex, ydex|
        xarr       = @xgrid[xdex]
        yarr       = @ygrid[ydex]
        if yarr && xarr
          yarr.each do |e| 
            next unless xarr.member?(e) 
            result << e unless result.member?(e)
          end  
        end   
      end
      return result
    end
  end
 end
       