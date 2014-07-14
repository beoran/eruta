# Long text widget.


module Zori
  class LongText
    include Widget
    
    BUTTON_BACKGROUND = [0x33, 0x33, 0xff, 0xaa]
    HOVER_BACKGROUND  = [0x55, 0x55, 0xff, 0xaa]

    def initialize(params={}, &block)
      super(params, &block)
      text                 = params[:text]
      @margin              = params[:margin] || 4
      @text                = text
      @bg                  = graph_box(@x, @y, @w, @h)
      @bg.border_thickness = 0
      @bg.border_color     = [255, 255, 255, 128]
      @bg.background_color = BUTTON_BACKGROUND
      
      @tg                  = graph_longtext(@x + @margin, @y  + @margin, 
                                            @w - (2*@margin) , @h - (2*@margin), text)
      # XXX fix on C side
      # @tg.text_flags       = Eruta::ALIGN_RIGHT
      @tg.font             = Eruta::Zori.font.id
      @tg.background_color = [0,0,0]
      @tg.color            = [255,255, 64]      
      @state.set(:hover, :false)
    end

    def on_mouse_axes(t, x, y, z, w, dx, dy, dz, dw)
      if @state.drag? 
         move_to(x, y)
      end
      
      # Check for hovering.
      if self.inside?(x, y)
        @bg.border_thickness = 1
        @bg.background_color = HOVER_BACKGROUND
        @state.set(:hover, true)
      else
        @bg.border_thickness = 0
        @bg.background_color = BUTTON_BACKGROUND
        @state.set(:hover, false)
      end
      return false # don't consume the event
    end
    
    def on_mouse_button_down(t, x, y, z, w, b)
      return false unless self.inside?(x, y)
      if @action
        @action.call(self)
      else
        @state.set(:drag, true)
        puts "Click! #{@state.drag?}"
      end
    end
    
    def on_mouse_button_up(t, x, y, z, w, b)
      @state.unset(:drag)
      puts "Release! #{@state.drag?}"
      return false unless self.inside?(x, y)
    end
        
    def can_drag?
      return true
    end
    
    
  end
end
