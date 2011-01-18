module Bump

  # A space is a container for Mobile and fixed objects. 
  # A space keeps track of what is where to perform broad-phase 
  # collision detection
  
  class Space
    attr_reader :gridsize
    
    def initialize(gridsize = 100)
      @mobiles = {}
      @fixed   = {}
      @gridsize= gridsize
      @xgrid   = {}
      @ygrid   = {}
    end
    
    def add_mobile(mobile, name = nil )
      name                ||= mobile.name
      @mobiles[name.to_sym] = mobile
      gridx                 = to_grid(mobile.x)
      gridw                 = to_grid(mobile.w)
      gridy                 = to_grid(mobile.y)
      gridh                 = to_grid(mobile.h)
      
      
    end
    
    def to_grid(value)
      return value.to_i / self.gridsize
    end
    
    def to_grid_x(mobile)
      return mobile.x / self.gridsize
    end
    
    def to_grid_x(mobile)
      return mobile.y / self.gridsize
    end
    
    
  end
end