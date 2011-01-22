module Bump

  # A space is a container for Mobile and fixed objects. 
  # A space keeps track of what is where to perform broad-phase 
  # collision detection
  
  class Space
    attr_reader :gridsize
    
    def initialize(gridsize = 100)
      @mobiles = {}
      @fixed   = {}
      @grid    = Grid.new(gridsize)
    end
    
    def add_mobile(mobile)
      @mobiles << mobile    
    end
    
    def add_fixed(thing)
       @fixed  << thing    
    end
    
    def prepare(dt)
      @mobiles.each do |mobile|
        mobile.prepare(dt)
      end
    end
    
    def handle_collisions(dt)
      to_check = @mobiles.dup
      # check all mobiles. O(n.log(n)), since we remove a mobile 
      # already checked 
      @mobiles.each do |mobile|
        mobile.collide_all(dt, to_check)
        to_check.delete(mobile)
      end
    end
    
    def update(dt)
      @mobiles.each do |mobile|
        mobile.update(dt)
      end
    end
    
  end
end