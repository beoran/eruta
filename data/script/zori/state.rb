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
        comp.hide
      end
      @state = :hidden
      unmark
      unhover
      hide_graph
    end

    # Shows the widget and all it's children
    def show
      @components.each do |comp|
        comp.show
      end
      @state = :active
      unmark
      unhover
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

    # Enables the widget and all it's on the children.
    def enable
      @components.each do |comp|
        comp.enable
      end
      unmark
      unhover
      @state = :active
      show_graph
    end
    
    # Enables the widget and all it's on the children.
    def disable
      @components.each do |comp|
        comp.disable
      end
      @state = :disabled
      unmark
      unhover
      show_graph      
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


  end
end
