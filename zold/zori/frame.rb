# A frame is simply a box with an outline that is useful as a container for other widgets
module Zori
  class Frame < Zori::Widget
   
    def initialize(*args)
      super(*args)
      @can_drag                 = true
      @can_focus                = false        
    end
    
    def draw_background(target)
      self.style.draw_background_image(target, self)
    end
    
  end
end
 