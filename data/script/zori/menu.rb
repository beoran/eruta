
module Zori
  class Menu < Widget
    
    attr_accessor :toplevel
    
    def initialize(*args, &block)
      super(*args, &block)
      # @z        =  10
      @toplevel = false
      @can_focus= true             
    end
    
    def add_child(child)
      super(child)
      # child.z = self.z + 10 # make sure child is on top      
    end
    
    alias :<< add_child

    def draw_border(target)
      # self.style.draw_outset(target, self)
    end
    
    # Hides this menu if it is not toplevel 
    def collapse()
      return false if @toplevel
      self.hide
      self.to_between
    end
    
    # Hides other sub-menus
    def hide_other_submenus(not_me)
      for child in self.children
        next if child == not_me
          tohide = child.children.first
          tohide.hide if tohide
      end
    end 
    
    def on_mouse_out(*args, new_widget)
      super(*args)
      return false if self.has_child?(new_widget)
      # Don't collapse if we're leaving for a child widget
      self.collapse
      # Hide if the menu is left by the mouse 
      # unless if we are toplevel. XXX: doesn't work well.
      # We have to convert the Hanao to keep a kind of "mouse over stack", 
      # in stead of a single item. 
    end
    
    def on_click(*args)
      ok = super(*args)
      return false unless ok
      self.collapse 
      # Hide if the menu is clicked, but not the menu item,
      # unless if we are toplevel.
    end
    
    # Pass on focus to first child widget if any
    def on_focus(from)
      return false unless self.children.first
      self.children.first.focus!
    end
    
    def draw_background(target)
      self.style.draw_background_image(target, self)
    end
    
  end
  
  class Submenu < Menu
    include Zori::Stacklayout
    def initialize(*args, &block)
      super(*args, &block)
      @toplevel = false
      self.hide
      # XXX this is a hack. The menu gets hidden but not deactivated properly 
      # on setup, only after being shown and hidden once

    end    
  end
  
  class Menubar < Menu
    include Zori::Flowlayout
    def initialize(*args, &block)
      super(*args, &block)
      @toplevel = true
    end
  end
  
end  
  
  