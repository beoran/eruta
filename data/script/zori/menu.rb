
module Zori
  # A menu is a control that can contain several rows and/or columns 
  # of buttons
  class Menu 
    include Widget
    attr_reader
    
    
    def initialize(params={}, &block)
      @selected= nil 
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

    def select_previous
      size = @components.size
      return nil if size < 1
      @selected -= 1
      if @selected < 0
        @selected = size - 1
      end
      Zori.root.mark_widget(@components[@selected])
      return @selected
    end
    
    def select_next
      size = @components.size
      return nil if size < 1
      @selected += 1
      if @selected >= size
        @selected = 0
      end
      Zori.root.mark_widget(@components[@selected])
      return @selected
    end

    def activate_selected
      size = @components.size
      return nil if size < 1
      @components[@selected].trigger
    end

    def on_key_down(*args)
      time, key = *args
      case key
        when KEY_UP
          select_previous
        when KEY_DOWN
          select_next
        when KEY_ENTER, KEY_SPACE
          activate_selected
        else
        p "#{self} on_key_down", time, key
      end
        
      
    end
    
    def can_drag?
      return false
    end

    def fit_children
      super
      if @components.first
        @selected = 0
        Zori.root.mark_widget(@components.first)
        @components.first.mark
      end
    end
    

    
  end
end
