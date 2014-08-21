
module Zori
  # A menu is a control that can contain several rows and/or columns 
  # of buttons
  class Menu 
    include Widget

    # returns the last/currently marked item for the menu. 
    attr_reader :marked
    
    
    def initialize(params={}, &block)
      @marked  = 0
      super(params, &block)
      @heading = params[:heading]
      @bg      = graph_box(x, y, w, h)
      @bg.border_thickness = 0
      @bg.border_color     = [255, 255, 255, 128]
      @bg.background_color = [0, 0, 255, 128]
      if @heading
        @tg                  = graph_text(x, y, w, h, @heading)
        @tg.margin           = 3
        @tg.text_flags       = Eruta::ALIGN_CENTER
        @tg.font             = Eruta::Zori.font.id
        @tg.background_color = [0,0,0]
        @tg.color            = [255,255, 64]
      end
      @action  = block
      self.unhover
    end
    
    def on_resize
      @bg.position= @x, @y
      @bg.size    = @w, @h
    end

    def mark_index(index)
      return nil if disabled?
      size = @components.size
      return nil if size < 1
      return nil if index < 0
      return nil if index >= size
      @marked = index
      Zori.root.mark_widget(@components[@marked])
      @components[@marked].mark
      return @marked
    end

    def mark_previous
      return nil if disabled?
      size = @components.size
      return nil if size < 1
      newmark = @marked - 1
      if newmark < 0
        newmark = size - 1
      end
      return mark_index(newmark)
    end
    
    def mark_next
      return nil if disabled?
      size = @components.size
      return nil if size < 1
      newmark = @marked + 1
      if newmark >= size
        newmark = 0
      end
      return mark_index(newmark)
    end

    def mark_recall
      return nil if disabled?
      newmark = @marked || 0
      return mark_index(newmark)
    end

    def mark_first
      return nil if disabled?
      return mark_index(0)
      return true
    end

    def activate_marked
      return nil if disabled?
      size = @components.size
      return nil if size < 1
      @components[@marked].trigger
      return true
    end

    def unselect_marked
      return nil if disabled?
      size = @components.size
      return nil if size < 1
      @components[@marked].unselect
      return true    
    end

    def previous_menu
      return nil if disabled?
      if @parent && @parent.parent && @parent.parent.is_a?(Zori::Menu)
        self.hide
        @parent.parent.unselect_marked
        @parent.parent.mark_recall
      end
      return true
    end

    def on_key_down(*args)
      time, key = *args
      case key
        when KEY_UP
          return mark_previous
        when KEY_DOWN
          return mark_next
        when KEY_ENTER, KEY_SPACE, KEY_LCTRL, KEY_RCTRL
          return activate_marked
        when KEY_BACKSPACE, KEY_LSHIFT, KEY_RSHIFT
          return previous_menu
        else
          p "#{self} on_key_down", time, key
      end
        
      
    end

    def on_enter
      return false if disabled?
      mark_recall
      return false
    end
    
    def can_drag?
      return false
    end

    def fit_children
      super
      mark_first
    end
    

    
  end
end
