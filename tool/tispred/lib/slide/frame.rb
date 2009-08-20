# Wrapper for JFrame and JPanel
# 
require 'swing'
require 'slide/wrapper'


module Slide
  class Frame
    include Slide::Wrapper
    attr_reader :menubar
    def initialize(title = "Untitled")
      @swing    = Swing::JFrame.new(title)      
      @menubar  = nil
    end
    
    def show
      @swing.setVisible(true)
    end
    
    def menubar=(bar)
      @menubar = bar
      @swing.setJMenuBar(@menubar.swing)
      return @menubar
    end
    
    def add_menubar(target)
      bar = Slide::Menubar.new(target)
      self.menubar = bar
    end
    
    def add_menu(title)
      return nil unless @menubar 
      return @menubar.menu(title)
    end
    
  end
  
  class Panel
    include Slide::Wrapper
    def initialize
      @swing = Swing::JPanel.new()
    end
    
    def layout=(new_layout)
      component     = Slide::Wrapper.to_component(new_layout)
      @swing.layout = component
    end
    
  end
    
end
