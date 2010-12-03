# A mixin module for Widget, that endows it with enhanced, yet trivial
# size and position methods, such as .right, .left . in_w (inner width), etc.
module Zori
  module Sizemixin
  
    # Outside width, that is, width plus the margins
    def out_w
      return self.w + (self.margin * 2)
    end
    
    # Inside width, that is, width minus the paddings 
    def in_w
      return self.w - (self.padding * 2)    
    end
    
    # Outside height, that is, width plus the margins
    def out_h
      return self.h + (self.margin * 2)
    end
    
    # Inside height, that is, width minus the paddings 
    def in_h
      return self.h - (self.padding * 2)    
    end
    
    # Left hand side of the widget 
    def left 
      return self.x
    end
    
    # Right hand side of the widget
    def right
      return self.x + self.w
    end
    
    # Top side of the widget 
    def top 
      return self.y
    end
    
    # Bottom hand side of the widget (position plus height)
    def bottom
      return self.y + self.h
    end
    
    # Middle of the widget on the x axis
    def middle
      return self.x + (self.w / 2)
    end
    
    # vertical center of the widget
    def vertical_center
      return self.y + (self.h / 2)
    end
    
    # Inner left hand side of the widget  
    def in_left 
      return self.x + self.padding
    end
    
    # Inner right hand side of the widget (position plus width minus padding)
    def in_right
      return self.x + self.w - self.padding
    end
    
    # Inner top side of the widget 
    def in_top 
      return self.y + self.padding
    end
    
    # Inner bottom hand side of the widget (position plus height)
    def in_bottom
      return self.y + self.h - self.padding
    end
    
    # Outer left hand side of the widget (position minus margin) 
    def out_left 
      return self.x - self.margin
    end
    
    # Outer right hand side of the widget (position plus width plus margin)
    def out_right
      return self.x + self.w + self.margin
    end
    
    # Outer top side of the widget 
    def out_top 
      return self.y - self.margin
    end
    
    # Outer bottom hand side of the widget 
    def out_bottom
      return self.y + self.h + self.margin
    end
    
    alias :outside_bottom :out_bottom
    alias :outside_top    :out_top
    
  
  end
end