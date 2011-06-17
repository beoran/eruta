module Zori
  class Menuitem < Button
   
    def initialize(*args, &block)
      action = lambda do |m|
        if m.parent
          m.parent.hide_other_submenus(m) 
          # hide other open submenus if any
        end
      
        submenu = m.children.first
        # Toggle our submenu's visibility if we have one
        if submenu 
          submenu.toggle_hidden
        end
        perform_action(block)
        if self.parent 
          self.parent.collapse
          # Hide our parent menu unless it is a toplevel menu
        end
      end
      @margin   = 0
      @padding  = 0
      super(*args, &action)
    end
    
    # Children of menuitems are hidden by default
    def add_child(child)
      child.hide
      # child.z = self.z + 10 # make sure child is on top
      child.x = self.x # overlay x of submenu
      super(child)
    end
    
    alias :<< add_child
    
    def draw(target)
        draw_heading(target, false)
    end
    
    # Pass on focus to first child widget if any
    def on_focus(from)
      return false unless self.children.first
      self.children.first.focus!
    end
    
    def on_click(*args)
      super(*args)
      self.to_front() # raise ourself to front.
    end
   
  end
  
  # A menu item that pops up a new vertical sub-menu
  class Submenuitem < Menuitem
    attr_reader :menu
    
    def initialize(*args, &block)
      super(*args, &block)
      @menu = Zori::Submenu.new
      add_child_self(@menu)
      @menu.hide   
    end
    
    # old add child redefined
    alias add_child_self add_child
    
    # add all childs to own sub-menu in stead, unless if menu is not set
    def add_child(widget)      
      @menu.add_child(widget)
      # Add child to sub menu
      @menu.layout_children
      @menu.fit_child_wide_simple
      @menu.fit_child_high_simple
      # And update the layout
    end
    
    alias :<< add_child
  
  end
end