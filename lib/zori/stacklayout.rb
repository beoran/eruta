module Zori
  module Stacklayout 
    # Mix in for classes that order their children top to bottom
    def layout_children
      case self.style.alignment 
        when :right
          return layout_children_right
        when :middle
          return layout_children_middle
        when :left
          return layout_children_left
        else
          return layout_children_left
      end
    end
    
    def layout_children_left
      left      = self.in_left #  self.x + self.padding 
      y         = self.in_top # y + self.padding
      for child in self.children do
        y      += child.margin        
        x       = left + child.margin 
        child.change_layout(x, y)
        y      += child.h + self.padding + child.margin
        child.update
      end
    end
    
    def layout_children_right
      right     = self.in_right   #  x  + self.w - self.padding
      y         = self.in_top     # 
      for child in self.children do
        y      += child.margin
        x       = right - child.w - child.margin
        # TODO: align center and right based on style.alignment.
        child.change_layout(x, y)
        y      += child.h + child.margin
        child.update
      end
    end
    
    def layout_children_middle
      middle    = self.middle   # self.x    + (self.w / 2) 
      y         = self.in_top   # self.y + self.margin
      for child in self.children do
        y      += child.margin        
        x       = middle - (child.w / 2) 
        # TODO: align center and right based on style.alignment.
        child.change_layout(x, y)
        y      += child.h + child.margin
        child.update
      end      
    end
    
    # Shrinks or grows self so all of self's child wigets will snugly fit inside 
    # self's height. This method doesn't move the widget around.
    # Always returns self for method chaining.    
    def fit_child_high()
      newh= 0
      for child in self.children do
        newh += child.out_h
      end
      self.h = newh
      return self
    end
    
    
  end
end