
module Zori

  class Placeholder < Widget
    def initialize(*args, &block)
      super(*args, &block)
      @can_focus= false
      @active   = false
    end
    
    def draw_self(target)
    end
    
    def draw_background(target)
    end    
  end
  
  #
  # The root level widget. Has special placeholder widgets to ensure 
  # dialogs get priority.
  # 
  class Mainwidget < Widget
    # Dialogs in this gui
    attr_reader :dialogs
    # Widgets in this GUI
    attr_reader :widgets

    alias add_child_self add_child

    def initialize(*args, &block)
      super(*args, &block)
      
      @dialogs   = Zori::Placeholder.new()
      @widgets   = Zori::Placeholder.new()
      # @dialogs.z = 5000
      self.add_child_self(@widgets)
      self.add_child_self(@dialogs)
      # Dialogs added last, logically, but they will be drawn over everything
      # if they are raised with to_front
      
      
      @can_focus = false
      @active    = false
    end

    def add_child
      return @widgets.add_child(child)    
    end

    def add_dialog(dialog)
      return @dialogs.add_child(dialog)
    end
    
    def draw_self(target)
    end
    
    def draw_background(target)
    end  

  end
end  