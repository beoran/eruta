
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
        @tg      = graph_text(x + (w / 2), y + 1, heading)
        @tg.text_flags = Eruta::ALIGN_CENTER
        @tg.font = Eruta::Zori.font.id
        @tg.background_color = [0,0,0]
        @tg.color            = [255,255, 64]
      end
      @action  = block
      @state.set(:hover, :false)
    end
    
    def on_resize
      @bg.position= @x, @y
      @bg.size    = @w, @h
    end

    def on_key_down(*args)
      time, key = *args
      p "#{self} on_key_down", time, key
      case key
      when KEY_UP
      else
      end
        
      
    end
    
    def can_drag?
      return false
    end
    

    
  end
end