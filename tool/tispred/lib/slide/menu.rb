# Simple wrappers for the menu-related SWING classes

require 'slide/wrapper'
require 'slide/action'
  

module Slide
  class Menuitem
    include Slide::Wrapper
    
    def initialize(label, iconname = nil, action_name = :action, &block )
      if iconname
        @icon    = Awt::ImageIcon(iconname).new
        @swing   = Swing::JMenuItem.new(label, icon)
      else
        @swing   = Swing::JMenuItem.new(label)        
      end
      # Forward actions to the block, if given
      if block
        self.action(action_name) do |act|
          block.call(act)
        end
      end
    end
  end
  
  class Menu
    include Slide::Wrapper
    def initialize(label, target)
      @target = target 
      @swing  = Swing::JMenu.new(label)
    end    
    def menuitem(label, iconname=nil, action_name = :action, &block)
      item = Menuitem.new(label, iconname, action_name)
      if block
        item.action(action_name) do |act|
          block.call(act)
        end
      end      
      self.add(item)
      return self
    end    
  end
  
  class Menubar
    include Slide::Wrapper
    def initialize(target)
      @target= target 
      @swing = Swing::JMenuBar.new()
    end  
    
    def menu(label) 
      menu = Menu.new(label, @target)
      self.add(menu)
      return menu
    end    
  end
  
  
end
