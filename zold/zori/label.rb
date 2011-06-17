
module Zori
  class Label < Zori::Widget
    attr_reader :heading
    
    def initialize(heading=nil, &block)      
      super(&block) 
      @heading      = heading || ""
      @pushed       = false
      @textsurf     = nil
      self.w        = Zori::Text.best_width(@heading, self.style) 
                    + self.padding * 4
      # Calculate what might be the best width      
      self.update()
      # @text         = Zori::Text.new(@heading, self.w, self.style)
      # self.h        = @text.high
    end
    
    def heading=(h)
      @heading = h
      @changed = true
      update
    end
        
    def update()
      @text           = Zori::Text.new(@heading, self.w, self.style)
      # XXX: it seems grossly inefficient that on every change, 
      # we have to create a new text... Zori::Text should be mutable.
      self.h         = @text.high
    end
        
    def draw(target)
      @text.draw(target, self.x, self.y)       
      # @text.draw(target, self.x + self.padding, self.y + self.padding)       
    end
    
    def on_scroll(direction)
      @text.scroll(direction)
    end
  end
end
