module Zori
  # A widget may be draggable
  module Draggable
    def on_mouse_axes(t, x, y, z, w, dx, dy, dz, dw)
      super rescue nil
      # super if defined? super doesn't seem to work yet or maybe I should
      # update my ruby
      if drag? 
         move_to(x - @drag_dx, y - @drag_dy)
      end
      return false # don't consume the event
    end
    
    def on_mouse_button_down(t, x, y, z, w, b)
      super rescue nil
      return false unless self.inside?(x, y)
      unless @action
        self.drag
        @drag_dx = x - self.x
        @drag_dy = y - self.y  
        puts "Click! #{drag?}"
        return true
      end
      return false
    end
    
    def on_mouse_button_up(t, x, y, z, w, b)
      super rescue nil
      self.drop if self.drag?
      puts "Release! #{drag?}"
      return false unless self.inside?(x, y)
    end
        
    def can_drag?
      return true
    end
    
  end
end
