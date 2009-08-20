# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 
require  'slide'

module Slide
  class Panel 
    include Slide::Wrapper
    def initialize
      @swing = Swing::JPanel.new
    end
    
    def layout=(new_layout)
      @swing.layout = new_layout
    end
    
  end
end
