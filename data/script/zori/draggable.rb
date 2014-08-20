module Zori
  # A widget may be draggable
  module Draggable
    def on_mouse_axes(t, x, y, z, w, dx, dy, dz, dw)
      super
      if drag? 
         move_to(x, y)
      end
      return false # don't consume the event
    end
    
    def on_mouse_button_down(t, x, y, z, w, b)
      super
      return false unless self.inside?(x, y)
      unless @action
        self.drag
        puts "Click! #{drag?}"
        return true
      end
      return false
    end
    
    def on_mouse_button_up(t, x, y, z, w, b)
      super
      self.drop if self.drag?
      puts "Release! #{drag?}"
      return false unless self.inside?(x, y)
    end
        
    def can_drag?
      return true
    end
    
  end
end
