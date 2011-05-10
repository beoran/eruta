require 'forwardable'

module Zori
  module Flowlayout
    # Mix in for classes that order their children top to bottom
    
    # Helper class for ordering widgets into lines 
    class Line
      extend Forwardable
      attr_reader :w
      attr_reader :h
      def initialize(*widgets)
        @line   = []
        @h      = 0
        @w      = 0
        if widgets
          widgets.each { |w| self << w }
        end   
      end
      
      def <<(widget)
        @line << widget
        @w    += widget.w + ( widget.margin * 2)
        # The line becomes wider with the width of the widget plus margins.
        wh     = widget.h + ( widget.margin * 2)
        @h     = wh if wh > @h 
        # The new height is that of the widget if the widget is the tallest
      end

      def_delegators :@line, :each, :empty?, :size
      
      # Layouts all children in the line 
      def layout_children(x, y)
        for child in @line do
          x      += child.margin
          child.change_layout(x, y)
          x      += child.w + child.margin 
          # Move to the left
          child.update
        end
      end

    end
    
    # Splits this widget's children up into lines that (should) fit 
    # the width of this widget 
    def children_to_lines
      max_wide        = self.in_w # self.w - (self.padding * 2)
      lines           = []
      line            = Zori::Flowlayout::Line.new() 
      # XXX: does this algorithm work well in all corner cases? Should check it!
      for child in self.children do
        child_wide    = child.out_w # child.w + (child.margin * 2)
        if line.w + child_wide  > max_wide 
        # Line width plus child width is too wide. 
        # Insert the line, and put the child in the next line  
          lines      << line
          line        = Zori::Flowlayout::Line.new(child)
        else # Not too wide, add child to the line
          line       << child
        end
      end
      # Don't forget to add the final line if not empty! 
      lines          << line unless line.empty?
      return lines
    end
   
    # Lays out a single line of children
    def layout_children_line(y, line)
      case self.style.alignment 
        when :right
          return layout_children_line_right(y, line)
        when :middle
          return layout_children_line_middle(y, line)
        when :left
          return layout_children_line_left(y, line)
        else
          return layout_children_line_left(y, line)
      end
    end
    
    # Lays out the children of this widget in a flow style
    # That is , one by one from left to right, and then on to the next line 
    # if the line is full.
    def layout_children
      lines           = children_to_lines
      y               = self.y + self.padding
      for line in lines do
        layout_children_line(y, line)
        y            += line.h
      end 
    end
    
    def layout_children_line_left(y, line)
      left      = self.in_left # self.x + self.padding
      x         = left
      line.layout_children(x, y) 
    end
    
    def layout_children_line_right(y, line)
      right     = self.in_right # self.x  + self.w - self.padding
      x         = right - line.w # Start as rightmost as possible
      line.layout_children(x, y)
    end

    
    def layout_children_line_middle(y, line)
      middle    = self.middle # self.x    + (self.w / 2)
      x         = middle    - (line.w / 2)
      line.layout_children(x, y)  
    end
    
    # Shrinks or grows self so all of self's child wigets will snugly fit inside 
    # self's height. This method doesn't move the widget around.
    # Always returns self for method chaining.    
    def fit_child_high()
      lineh= 0
      newh = 0
      linew= 0
      maxw = self.in_w # - (self.padding * 2)
      for child in self.children do
        childh = child.out_h
        childw = child.out_w
        lineh  = childh  if childh > lineh         
        if linew + childw >= maxw
          linew = 0
          newh += lineh
          lineh = 0
        else 
          linew += childw   
        end        
      end
      p "fit high"      
      newh += lineh
      p lineh, newh
      self.h = newh      
      return self
    end


  end
end