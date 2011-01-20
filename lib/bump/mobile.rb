module Bump
  # Anything that can move with regards to collision detection.
  module Mobile
    include Thing
    attr_accessor :dp # distance vector that the center will be moved by 
    # in the next time step dt
    attr_accessor :v  # speed vector
    attr_accessor :m  # mass
    
    def initialize(opt={})
      super(opt)
      @v = Bump.vec(opt[:vx] || 0.0, opt[:vy] || 0.0)
      @dp= Bump.vec0
      @m = opt[:m] || 1.0
    end
    
    
    def prepare_p(dt)
      @dp = (@v * dt)
    end
    
    def update_p
      @p += @dp
    end
   
    def prepare(dt)
      prepare_p dt
    end
    
    def update
      update_p
    end
 
    # relative speed towards other mobile
    def vrel(other)
      return self.v - other.v
    end
    
  
  end
end