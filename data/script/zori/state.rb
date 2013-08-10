module Zori
  # State manages the current state of a widget or Hanao.
  class State
    
    # Initializes the State
    # States are mutable so we need setters, but we use a hash storage 
    # for flexibility. 
    def initialize(stats={})
      @states = { :hover => false, :focus => false, :drag => false, :active => true }
      @states = @states.merge(stats)
    end
    
    # Checks if a certain state is set
    def is?(state)
      return @states[state.to_sym]
    end
    
    # Sets a certain state, by default sets it to true.
    # It's allowed to store objects in the state too.
    def set(state, value = true)
      return @states[state.to_sym] = value
    end
    
    # Unsets a certain state.
    def unset(state)
      set(state, false)
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
    
    # Is the object being clicked?
    def click?
      return is?(:click)
    end
   
    # Is the object being hovered?
    def hover?
      return is?(:hover)
    end

  end
end
