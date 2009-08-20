
 

module Slide
  module Wrapper
    
    # Class function to check an object and "transform" it into an AWT/Swing 
    # component
    def self.to_component(object)
      if object.is_a? Awt::Component        
        return object
      elsif object.respond_to? :swing
        # Otherwise, try to get the .swing value
        return object.swing
      else 
        return nil
      end      
    end
    
    
    attr_reader :swing
    def initialize
      @swing = nil
    end
    
    def add(other)
      component = Slide::Wrapper.to_component(other)
      if component
        @swing.add(component)
      else 
        raise "Cannot add #{other} to this UI element."
      end      
    end
    
    def resize(*sizes)
      w = sizes.shift
      return unless w
      h = sizes.shift || w
      @swing.setSize(w, h)      
    end
    
    # Enables mouse handling
    def enable_mouse
      return @mouse if @mouse
      @mouse ||= Slide::Mouse::Adapter.new
      @swing.addMouseListener(@mouse)
      @swing.addMouseMotionListener(@mouse)
      return @mouse
    end
    
    # Gets the mouse handler. 
    def mouse
      return enable_mouse
    end
    
    # Enables action handling
    def enable_action(name = :action, block = nil)
      sname = name.to_sym
      return @action[sname] if (@action and @action[sname]) 
      @action       ||= {} 
      @action[sname]  = Slide::Action::Adapter.new(sname)
      if block
        @action[sname].action do |act| block.call(act) end
      end
      @swing.addActionListener(@action[sname])
      return @action[sname]
    end
    
    # Gets the action handler for this action name (:action) by default. 
    def action(name=:action, &block)
      return enable_action(name, block) 
    end

    
    
    def next_to(other)
      @swing.setLocationRelativeTo(other.swing)
    end
    
  end
end
