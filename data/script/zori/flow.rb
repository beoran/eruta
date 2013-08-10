# A flow is a simple, empty container with a flow layout
module Zori
  class Flow < Widget
    
    include Flowlayout
    
    def initialize(*args)
      super(*args)
      @can_drag                 = true
      @style                    = self.style.deep_copy
      # @style.colors.background  = Sisa::Color::SkyBlue
      @active                   = false   # Don't interact
    end
    
    def draw(target)
      # draw nothing
    end
    
    def draw_background(target)
      super(target)
      # don't even draw a background
    end
    
  end
end   
