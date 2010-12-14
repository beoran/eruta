

module Zori
  class Checkbox < Zori::Widget
    attr_accessor :value
    attr_accessor :checked
  
    def initialize(&block)
      super(&block)
      self.value    = nil 
      self.checked  = false
      self.w        = self.h = self.font.height # - self.margin * 2
    end
       
#     def heading=(h)
#       @heading = h
#       @changed = true
#       update
#     end    
#    
#     def update()
#       if @heading
#         linewide, linehigh  =  *self.font.size_text(@heading)
#         self.w              = linewide  + (self.padding * 2)
#         self.h              = linehigh  + (self.padding * 2)
#       end  
#     end
    
    def draw_check(target)
      tx    = self.colors.shadow  
      left  = self.x + self.padding
      right = self.x + self.w - self.padding
      top   = self.y + padding 
      wide  = self.w - (2 * self.padding )
      high  = self.h - (2 * self.padding )
      target.slab(left, top, wide, high, tx)
    end
    
    def draw_pressed(target)
      fg = self.colors.highlight      
      target.slab(self.x, self.y, self.w, self.h, fg)
      self.put_inset(target)
      draw_check(target)    
    end
    
    def draw_released(target)
      bg = self.colors.background
      target.slab(self.x, self.y, self.w, self.h, bg)
      self.put_outset(target)
    end
    
    def draw(target)
      if (self.checked)
        draw_pressed(target)
      else
        draw_released(target)
      end      
    end
    
    # Toggles value of this check box
    def toggle
      self.checked = !self.checked   
    end
    
    # Called on key press 
    def on_key(code, mods, text)
      super(code, mods, text)
      if code == Sisa::Key::SPACE
        self.toggle
      end
    end

    
    def on_click(x, y, b)
      self.toggle
      act
    end
    
    
  end
end  
  
  
  