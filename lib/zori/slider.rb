
module Zori
  # Horizontal slider.
  class Slider < Zori::Widget
    attr_reader :value     # Current text value 
    
    def initialize(options = {}, &block)
      super(&block) 
      @min                = options[:min]         || 0
      @max                = options[:max]         || 100
      self.value          = options[:value]       || 0
      self.w              = @max      + (self.padding * 2)
      self.h              = (self.padding * 2)
      # Default size guess. 
      update()
    end
    
    def value=(val)
      return unless val
      val = @max if val > @max 
      val = @min if val < @min
      @value     = val
      @changed   = true
      update()
      act
    end
         
    # Notifies input box of changes to the text.
    def update()
          
    end    
    
    def w_min
      aid   = self.w
      aid   = 1 if aid < 1
      return aid
    end
    
    # Draw ourselves
    def draw(target)
      # update()
      fg = self.colors.text
      bg = self.colors.background      
      self.put_inset(target)
      delta = (self.value - @min) * self.w_min / @max 
      target.slab(self.x + delta, self.y, self.h, self.h, fg)
    end    
    
    # Handle mouse clicks
    def on_click(mx, my, b)
      super(mx, my, b)      
      self.value = ((mx - self.x) * (@max - @min)) / self.w_min
    end
    
    def on_mouse_drag(old_x, old_y, event_x, event_y)
      on_click(event_x, event_y, 1)
    end
    
    # handle mouse scrolls
    def on_scroll(val)
      self.value += val
    end
    
   
    # Handle key presses
    def on_key(key, modifier, string)
      case key
        when Gari::Key::KEY_RIGHT
          self.value += 1 
        when Gari::Key::KEY_LEFT
          self.value -= 1 
        when Gari::Key::KEY_HOME
          self.value = @min
        when Gari::Key::KEY_END
          self.value = @max
       else
      end
    end

  
  end
end