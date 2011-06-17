module Zori
  # Widget mixin that helps with layout abilities.
=begin
Idea on how to layout in a simple way:
* There are widgets that 'know" their size in advance and those that don't.
* A static label, a button with fixed text, etc, knows it's size.
  This may change if the label's text, etc changes, but that's uncommon.
* As a simplification, let's require that a leaf widget always must know 
  it's size. It's parent isn't allowed to change that directly.
* However, leaf widgets dont't know where in the parent they should go to fit.
* The branch widgets decide on position.
* Parents -> choose child's position. Child chooses size.
* However , child may disobey parent. 
=end
  module Layoutmixin
    
    # call this method to change the layout of a widget
    def change_layout(newx, newy)
      self.x = newx
      self.y = newy
      self.layout_all
    end
    
    # Lays out this object's children. Does nothing by default.
    # Is implemented by the selected layout mixin
    def layout_children
    end

    #  layouts children and self
    def layout_all
      self.layout_children
      self.layout
    end
    
    # layouts self
    def layout
    end
    
    # Simple height fitter that simply looks at the last (presumably bottom-most)
    # widget. Only works after the children are properly layout-ed
    def fit_child_high_simple()
      maxh    = 0
      self.children.each { |child| maxh = child.out_bottom if child.out_bottom > maxh } 
      newh    = maxh + self.padding
      self.h  = newh
      return self
    end
    
    # Simple width fitter. Only works after the children are properly layout-ed
    # and if the widget is left-aligned
    def fit_child_wide_simple()
      maxw    = 0
      self.children.each { |child| maxw = child.out_right if  child.out_right > maxw } 
      neww    = maxw + self.padding
      self.w  = neww
      return self
    end
    
        # Shrinks or grows self so it snugly fits inside the given widget's width. 
    # If widget is nil, fits itself to widget.parent. If that is also 
    # not set, does nothing. This method doesn't move the widget around.
    # Always returns self for method chaining.
    def fit_wide(widget=nil) 
      widget ||= self.parent     
      return self unless widget
      self.w   = widget.w - ( widget.padding * 2) - (self.margin * 2)
      return self
    end
    
    # Shrinks or grows self so it snugly fits inside the given widget's height. 
    # If widget is nil, fits itself to widget.parent. If that is also 
    # not set, does nothing. This method doesn't move the widget around.
    # Always returns self for method chaining.
    def fit_high(widget=nil) 
      widget ||= self.parent     
      return self unless widget
      self.h   = widget.h - ( widget.padding * 2) - (self.margin * 2)
      return self
    end


  end
end  