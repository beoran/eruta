# Button widget.

module Zori
  class Progress < Zori::Widget
    attr_reader     :value
    attr_reader     :maximum
    
    def initialize(max = 100, val = 0, &block)
      super(&block) 
      @maximum      = max || 100
      @value        = val || 0
      self.h        = 10 
      self.w        = max
    end
    
    def value=(val)
      @value = val
      @value = 0        if value < 0 
      @value = @maximum if value > @maximum
    end
    
    def advance(by=1)
      if self.value != self.maximum     
        self.value += by
        if self.value == self.maximum  # We just reached max, call action
          self.act
        end
      end  
    end
       
    def draw(target)
      wmax  = self.w - (self.margin * 2)
      hreal = self.h - (self.margin * 2)
      wreal = ((wmax * @value) / @maximum.to_f).round
      target.slab(x + self.margin, y + self.margin, 
                  wreal, hreal, self.colors.text)
      put_border(target)
    end
    
    def can_drag?
      return true
    end
    
  end
end
