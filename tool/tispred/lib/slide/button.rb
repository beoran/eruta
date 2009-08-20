# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 

module Slide
  class Button
    include Slide::Wrapper
    
    def initialize(text, icon_name  = nil)
      if icon_name
        @swing = Swing::JButton.new(text, icon_name)
      else
        @swing = Swing::JButton.new(text)
      end      
    end
    
    def background_rgba(r, g, b ,a)
      color = Awt::Color.new(r, g, b, a)
      @swing.setBackground color      
    end
    
    
    
  end
end
