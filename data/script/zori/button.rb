# Button widget.


module Zori
  class Button
    include Widget
    
    BUTTON_BACKGROUND = [0x33, 0x33, 0xff, 0xaa]
    HOVER_BACKGROUND  = [0x55, 0x55, 0xff, 0xaa]

    def initialize(params={}, &block)
      super(params, &block)
      heading              = params[:heading]
      @heading             = heading
      @bg                  = graph_box(@x, @y, @w, @h)
      @bg.border_thickness = 0
      @bg.border_color     = [255, 255, 255, 128]
      @bg.background_color = BUTTON_BACKGROUND
      
      @tg                  = graph_text(@x, @y, @w, @h, heading)
      @tg.text_flags       = Eruta::ALIGN_CENTER
      @tg.font             = Eruta::Zori.font.id
      @tg.background_color = [0,0,0]
      @tg.color            = [255,255, 64]
      @tg.margin           = 1
      self.unhover
    end

    
    def on_mouse_in(x, y, from)
      super(x, y)
      @pushed  = false # need to check for mouse pressed or not
    end
    
    def on_mouse_out(x, y, to)
      super(x, y)
      @pushed  = false
    end

    def marked_style
      @bg.border_thickness = 1
      @bg.background_color = HOVER_BACKGROUND
    end

    def unmarked_style
      @bg.border_thickness = 0
      @bg.background_color = BUTTON_BACKGROUND
    end

    def hover
      marked_style
      super
    end

    def unhover
      unless mark?
        unmarked_style
      end
      super
    end

    def mark
      marked_style
      super
    end

    def unmark
      unless hover?
        unmarked_style
      end
      super
    end
    

    
    def on_mouse_axes(t, x, y, z, w, dx, dy, dz, dw)
      # Check for hovering.
      if self.inside?(x, y)
        hover
      else
        unhover
      end
      return false # don't consume the event
    end

    def trigger
      if @components.first.is_a? Zori::Menu
        menu = @components.first
        menu.show
        menu.mark_recall
        self.select
        return true
      end
      super
      return true
    end
    
    def on_mouse_button_down(t, x, y, z, w, b)
      return false unless self.inside?(x, y)
      self.trigger
    end
    
    def can_drag?
      return false
    end
    

    
  end
end
