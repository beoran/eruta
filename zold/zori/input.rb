
module Zori
  class Input < Zori::Widget
    include Fixedlayout
  
    # Cursor for an input object
    class Cursor 
      
      attr_accessor :x 
      attr_accessor :y
      attr_accessor :h
      attr_accessor :w
      
      attr_reader   :string
      attr_reader   :index
      attr_reader   :row
      attr_reader   :column
      
      def initialize(str, ind = nil)
        @str      = str        
        @max      = str.length
        @index    = ind || @max
        @w        = 1
        @color    = Gari::Color.rgb(255, 255, 191)
        update()
      end
      
      def update()
        @row      = @str[0, @index].count("\n")
        @column   = @index - (@str[0, @index].rindex("\n") || @index)
      end
      
      def index=(i)
        @index    = i
        index     = @max if @index > @max 
        index     = 0    if @index < 0 
        update()
      end
      
      def forwards()
        self.index += 1
      end
      
      def backwards()
        self.index -= 1
      end    
        
      def draw(target)
        target.line(self.x, self.y, self.w, self.h, @color)
      end
    end
      
    attr_reader   :cursor    # Current cursor position
    attr_reader   :marker    # Current marker position, if there is a selection
    attr_reader   :value     # Current text value 
    attr_accessor :maxchar   # Maximum amount of characters
    attr_accessor :value     # current text value
    attr_accessor :valuewide # Width of value
    
    def initialize(text = '', &block)
      super(&block) 
      self.value          = text
      @cursor             = self.value.size # Cursor.new(@value)
      @scroll             = 0
      @maxchar            = 20
      linewide, @linehigh = *self.font.size_text("M" * (@maxchar+1))
      self.w              = linewide  + (self.margin * 2)
      self.h              = @linehigh + (self.margin * 2)
      # Default size guess. 
      update()
    end
    
    def text=(txt)
      
      
      
      self.value = txt
      @scroll    = 0
      @cursor    = self.value.size
      @changed   = true
      update()
    end
        
    # Update to be called when only the text has changed
    def update_text
      @valuewide, @valuehigh  = * self.font.size_text(self.value)
    end
    
    # Update to be called when only the cursor position changed, 
    def update_cursor
      cwide, chigh    = * self.font.size_text(self.value[0,@cursor])
      @cursor_x1      = self.x + cwide + self.margin
      @cursor_x2      = self.x + cwide + self.margin
      @cursor_y1      = self.y + self.margin
      @cursor_y2      = self.y + chigh # - self.margin
    end
    
    # Notifies input box of changes to the text.
    def update()
      update_text()
      update_cursor()            
    end    
    
    # Draw ourselves
    def draw(target)
      # update()
      fg = self.colors.text
      bg = self.colors.background      
      target.slab(self.x, self.y, self.w, self.h, bg)
      
      self.put_inset(target)
      dx = self.x + self.margin
      dy = self.y + self.margin      
      target.text(dx, dy, @value.to_s, font, fg, bg)
      
      #self.font.draw( target, self.x + self.margin, self.y + self.margin, 
      #                self.value, fg, bg )
      # draw cursor if we have focus
      if self.focus?
        col = self.style.colors.text
        ih  = self.h - (2 * self.margin) 
        target.line(@cursor_x1, @cursor_y1, 0, ih, col)
        # Todo: cursor should follow 
      end
    end    
    
    # Handle mouse clicks
    def on_click(mx, my, b)
      super(mx, my, b)      
      if self.valuewide and self.valuewide > 0
        p "#@scroll, #{mx} #{self.x} #{self.value.size} #{self.valuewide}"
        @cursor = @scroll +  ((mx - self.x) * self.value.size / (self.valuewide)).to_i
      else
        @cursor = 0
      end
      @cursor = 0 if @cursor < 0
      @cursor = self.value.size if @cursor > self.value.size
      # Calculate and clamp cursor
      update_cursor()
      # Calculate approximative new cursor position.
    end
    
    # Handle key repeats by forwarding them to key_down
    def on_key_repeat(key, mod, string)
      on_key(key, mod, string)
    end
    
    # Handle key presses
    def on_key(key, modifier, string)
      case key
        when Gari::Key::KEY_RIGHT
          if @cursor < self.value.size
            @cursor += 1
            update()
          end
        when Gari::Key::KEY_LEFT
           if @cursor > 0
            @cursor -= 1
            update()
           end
        when Gari::Key::KEY_HOME
          @cursor = 0
          update()
        when Gari::Key::KEY_END
          @cursor = self.value.size
          update()
        when Gari::Key::KEY_BACKSPACE
          if @cursor > 0
            @cursor -= 1
            self.value[@cursor] = '' # Remove character
            update()
           end 
        when Gari::Key::KEY_DELETE          
          if @cursor < self.value.size
            self.value[@cursor] = '' # Remove character
            update()
          end                    
        when Gari::Key::KEY_RETURN, Gari::Key::KEY_KP_ENTER
           act(:return)
        when Gari::Key::KEY_UP
           act(:up)
        when Gari::Key::KEY_DOWN
           act(:down)
           # Call the given block if enter  is pressed.          
        when Gari::Key::KEY_LSHIFT, Gari::Key::KEY_RSHIFT
          # Ignore control keys.
        else
          if string && (!string.empty?) && string != "\0" && (self.value.size < @maxchar)            
            if ((modifier & Gari::Key::KEY_LSHIFT) > 0 or 
                (modifier & Gari::Key::KEY_RSHIFT) > 0) 
              string.upcase! 
            end
            self.value.insert(@cursor, string)
            @cursor   += string.size
            update()
          end
      end 
      @changed = true
      # update()
    end
    
  end
end
