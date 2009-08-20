module Zori
  class Radiobutton < Checkbox
    
    def draw_pressed(target)
      fg = self.colors.highlight
      target.fill_circle(self.x, self.y, self.w, fg)
      target.put_circle(self.x, self.y, self.w, self.colors.highlight)
      target.put_circle(self.x + 1, self.y + 1, self.w - 2, self.colors.shadow)
      target.fill_circle(self.x + 4, self.y + 4, self.w - 8, self.colors.shadow)
    end
    
    def draw_released(target)
      bg = self.colors.background     
      target.fill_circle(self.x, self.y, self.w, bg)
      target.put_circle(self.x, self.y, self.w , self.colors.highlight)
      target.put_circle(self.x + 1, self.y + 1, self.w - 2, self.colors.shadow)
    end
    
    
  end
end