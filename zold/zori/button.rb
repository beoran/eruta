# Button widget.


require 'time'


module Zori
  class Button < Zori::Widget
    attr_reader     :heading
    attr_reader     :textarea
    
    def initialize(heading=nil, &block)
      super(&block) 
      @heading      = heading || "OK"
      @pushed       = false
      @textsurf     = nil
      if heading 
        linewide, linehigh =  *self.font.size_text(@heading)
        self.w      = linewide  + (self.padding * 2)
        self.h      = linehigh  + (self.padding * 2)
      else  
        self.w      = self.h    = 32  
      end
      update
    end
       
    def heading=(h)
      @heading = h
      @changed = true
      update
    end    
   
    def update()
      if @heading
        linewide, linehigh =  *self.font.size_text(@heading)
        self.w      = linewide  + (self.padding * 2)
        self.h      = linehigh  + (self.padding * 2)
      end  
    end
    
    def draw_heading(target, pressed = false)
      if heading
        dx = self.padding + (pressed ? 1 : 0)
        dy = self.padding + (pressed ? 1 : 0)
        self.put_text(target, self.x + dx, self.y + dy, heading)
      end
    end
    
    def draw_pressed(target)
      bg = self.colors.background
      self.put_background(target)
      self.put_inset(target)
      draw_heading(target, true)
    end
    
    def draw_released(target)
      bg = self.colors.background      
      self.put_background(target)      
      self.put_outset(target)
      draw_heading(target, false)
    end
    
    def draw(target)
      if(@mouse_down)
        draw_pressed(target)
      else
        draw_released(target)
      end      
    end
    
    def on_mouse_in(x, y, from)
      super(x, y)
      @pushed  = false # need to check for mouse pressed or not
    end
    
    def on_mouse_out(x, y, to)
      super(x, y)
      @pushed  = false
    end
    
    def on_click(x, y, b)
      ok = super(x, y, b)
      return false unless ok
      act
      return :stop # Don't let others see this click
    end
    
    def can_drag?
      return false
    end
    

    
  end
end
