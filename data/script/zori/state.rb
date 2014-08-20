module Zori
  # State manages the current state of a widget or Hanao.
  module State

    def initialize(params={}, &block)
      super(params, &block)
      @state  = :active
      @mark   = false
      @hover  = false
    end
    
    # Hides the widget and all it's children
    def hide
      @components.each do |comp|
        comp.hide_component
      end
      @state = :hidden
      unmark
      unhover
      hide_graph
    end

    # Hides a child component. This hides only the graph, and does not
    # touch the @hidden field
    def hide_component
      @components.each do |comp|
        comp.hide_component
      end
      hide_graph
    end

    # Shows the widget and all it's children
    def show
      @components.each do |comp|
        comp.show_component
      end
      @state = :active
      show_graph
    end

    # Shows a child component, but only if it isn't hidden itself.  
    def show_component
      if hidden?
        return
      end
      @components.each do |comp|
        comp.show_component
      end
      show_graph
    end



    
    # Moves the component and all it's children to (x, y)
    def move_to(x, y)
      @x = x
      @y = y
      @components.each do |comp|
        comp.move_to(x, y)
      end
      move_graph(x, y)
    end

    # Enables the widget, has no effect on the children
    def enable
      @state = :active
    end
    
    # Disables the widget, has no effect on the children.
    def disable
      @state = :disabled
    end
    
    # Marks the widget. Has no effect on the children.
    # Does nothing on non-active widgets.
    def mark
      return false if disabled?
      @mark = true
      return true
    end

    # Removes the mark from the widget. Has no effect on the children.
    # Does noting on non-marked widgets.
    def unmark
      return false unless mark?
      @mark = false
      return true
    end

    # Hovers the widget. Has no effect on the children.
    # Does nothing on non-active widgets.
    def hover
      return false if disabled?
      @hover = true
      return true
    end

    # Removes the hover flag from the widget. Has no effect on the children.
    # Does noting on non-marked widgets.
    def unhover
      return false unless hover?
      @hover = false
      return true
    end

    # Selects the widget. Has no effect on the children.
    # Does nothing on non-active widgets.
    def select
      return false if disabled?
      @select = true
      @select_mark          = Zori.root.create_select_mark
      @select_mark.position = self.left - 20, self.y
      graph_add(@select_mark)
      return true
    end

    # Removes the hover flag from the widget. Has no effect on the children.
    # Does noting on non-marked widgets.
    def unselect
      return false unless selected?
      @select = false
      graph_delete(@select_mark)
      @select_mark.close
      @select_mark = nil
      return true
    end

    # Focuses the widget. Has no effect on the children.
    # Does nothing on non-active or non-marked widgets.
    def focus
      return false if disabled?
      @state = :focus
      return true
    end

    # Removes the focus from the widget. Has no effect on the children.
    # Does noting on non-focused widgets.
    def unfocus
      return false unless focus?
      @state = :active
      return true
    end

    # Sets the dragging state to the widget. Has no effect on the children.
    # Does noting on disabled widgets.
    def drag
      return false if disabled?
      @state = :drag
      return true
    end

    # Removes the dragging state from the widget. Has no effect on the children.
    # Does noting on non-dragging widgets.
    def drop
      return false unless drag?
      @state = :active
      return true
    end
    
    # Checks if the widget is in a certain state.
    def is?(qstate)
      @state == qstate
    end

    def hidden?
      return is?(:hidden)
    end

    # Is the object disabled or hidden (which implies disabled)
    def disabled?
      return is?(:disabled) || is?(:hidden)
    end
    
    # Is the object being dragged?
    def drag?
      return is?(:drag)
    end
    
    # Does the object have focus?
    def focus?
      return is?(:focus)
    end
    
    # Is the object active?
    def active?
      return is?(:active)
    end
    
    # Is the object being triggered?
    def trigger?
      return is?(:trigger)
    end
   
    # Is the object being marked?
    def mark?
      return @mark
    end

    # Is the object being hovered?
    def hover?
      return @hover
    end

    # Is the object selected?
    def selected?
      return @select
    end



  end
end
