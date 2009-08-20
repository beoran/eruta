# encoding: UTF-8
# A Hanao is the environment (OS, Screen, Event queue)
# in which the user interface is displayed and with which 
# it interacts. (A Hanao is the sandal strap of a Zori.)
# Remark on the dision of labour between Hanao and Widget:
# Hanao handles everything that regards or influences different widgets,
# Widget handles everything that only regards or influences one widget.
# Hanao also contains various helper functions for drawing, etc. 

require 'forwardable'

module Zori
  class Hanao
    extend Forwardable
    
    # Constants    
    # Directory under which images are stored
    IMAGE_DIR     = File.join(* %w{.. share image ui} )
    p Dir.pwd
    
    
    # Time mouse button must be down to generate a click
    CLICK_TIME    = 0.5
    # Time keyboard button must be down to generate a repeat
    REPEAT_TIME   = 0.5 
    # Time between repeated keys
    REPEAT_SPEED  = 0.05 
    # Enable workaround for utf scancode bug on Ubuntu
    SCANCODE_BUG_WORKAROUND = true
    
    
    # Structure for mouse button info
    MouseButtonInfo = Struct.new(:button, :pressed, :released)
    # Structure for Click Info
    ClickInfo       = Struct.new(:button, :widget, :when)
    # Structure for Drag Info
    DragInfo        = Struct.new(:button, :widget, :when)
    
    def self.load_image(*names)
      return Sisa::Surface.load_alpha(File.join(IMAGE_DIR, *names))
    end  
    
    # Class that models the mouse state and constants
    class Mouse
      LEFT_BUTTON   = 1
      MIDDLE_BUTTON = 2
      RIGHT_BUTTON  = 3
      SCROLL_UP     = 4
      SCROLL_DOWN   = 5
      
      # Returns if a button is in reality a scroll or not. 
      # Needed due to an SDL artifact which translates scrolls to 
      # moude button presses and releases.
      # If it is a scroll, returns -1 for a scroll up, and +1 for a scroll down.
      # otherwise returns nil
      def self.is_scroll?(button)
        case button 
        when SCROLL_UP
          return -1
        when SCROLL_DOWN
          return +1
        else
          return nil
        end   
      end     
    
      attr_accessor :x, :y, :under, :clicked
      
      def initialize(xin = 0, yin = 0)
        @x       = xin
        @y       = yin
        @under   = []
        @clicked = nil
        @pressed = {}
        @dragged = {}
        @visible = true
        @cursor  = Hanao.load_image('cursor', 'mouse.png') rescue nil;
        Sisa::Mouse.hide if @cursor 
      end
      
      def move(xin, yin) 
        @x = xin
        @y = yin
      end
      
      def press(button, widget)
        # @pressed[button] ||= MouseButtonInfo.new()
        @pressed[button] = ClickInfo.new(button, widget, Time.now) 
      end
      
      def clicked?(widget)
        return @clicked == widget
      end
      
      def release(button)
        @pressed.delete(button);
        @clicked = nil
        @dragged = {}
      end
      
      def pressed?(button=nil)
        return (!@pressed.empty?) unless button
        return @pressed[button]
      end
      
      # Draws the mouse cursor to the screen
      def draw(screen)
        color = Sisa::Color::White
        color = Sisa::Color::Green if pressed?()
        color = Sisa::Color::Red   if pressed?(2)
        if @cursor
          screen.put_bitmap(@x, @y, @cursor)
        else   
          screen.fill_circle(@x, @y, 1, color)
        end  
#     Uncomment these lines below to mark clicked/hovered widgets for debugging
#         if @clicked
#            screen.fill_circle(@clicked.x, @clicked.y, 10, Sisa::Color::Red)
#         end
#         if @under && (!under.empty?)
#           for under in @under do
#             screen.fill_circle(under.x, under.y, 7, Sisa::Color::Green)
#           end 
#         end

        
      end
      
    end  
    
    # Class that models the state of a single key
    Keyinfo = Struct.new(:pressed, :sym, :mod, :text, :repeated)
         
    # Class that models the Keyboard state
    class Keyboard
      attr_reader :keys, :mods
      def initialize
        @keys = {}
      end
      
      def press(scancode, mod, text)
        # Reuse Keyinfo object, to lessen memory use
        unless @keys[scancode]  
          info            = Keyinfo.new()
          @keys[scancode] = info
        end
        @keys[scancode].pressed = Time.now
        @keys[scancode].sym     = scancode
        @keys[scancode].mod     = mod
        @keys[scancode].text    = text
        @keys[scancode].repeated= nil  
      end
      
      # Returns the keyinfo for the given scancode, or nil if not pressed yet.
      def state(scancode)
        return @keys[scancode]
      end
      
      # Returns a Time when the key was last pressed
      def pressed?(scancode) 
        s = self.state(scancode)
        return false unless state
        return state.pressed
      end
      
      # Registers a key release 
      def release(scancode)
        return unless @keys[scancode]
        @keys[scancode].pressed = nil 
        @keys[scancode].repeated= nil
      end
      
      # Returns keys that need to be repeated
      def to_repeat()
        result = []
        now   = Time.now
        for k, value in @keys do
          if value.pressed 
            if ! value.repeated && ((now - value.pressed) > Hanao::REPEAT_TIME) 
            # First key repeat 
              result << value
            elsif value.repeated && ((now - value.repeated) > Hanao::REPEAT_SPEED)
            # Consecutive key repeats
              result << value
            end  
          end    
        end
        return result
      end
      
    end
    
    # Class that models the Joystick state
    class Joystick
    
      attr_accessor :x
      attr_accessor :y
      attr_accessor :active
      
      def make_joystick_cursor
        return Sisa::Surface.load_alpha(File.join(IMAGE_DIR, 'cursor', 'joystick_0.png'))        
#         cursor = Sisa::Surface.make(16, 16)
#         col    = Sisa::Color::Black
#         cursor.put_line(0 , 16,  0, 32, col)
#         cursor.put_line(16,  0, 16, 16, col)
#         cursor.put_line(16, 32, 16, 16, col)
#         cursor.fill_rectangle(16, 16, 16, 16, col)
#         return cursor
      end
    
      def initialize
        @buttons = {}
        @axis    = {}
        @hats    = {}
        @balls   = {}
        @cursor  = make_joystick_cursor
        @x       = 100
        @y       = 100
        @active  = true
      end
      
      def draw(screen)
        screen.put_bitmap(@x, @y, @cursor)
      end
      
    end

    attr_reader   :screen     # Screen
    attr_reader   :queue      # Event queue
    attr_accessor :main       # Main widget
    attr_reader   :done       # Is the system requesting quit?   
    alias :done?  :done
    attr_reader   :active     # Is the system active?
    attr_reader   :hovered    # The widget we are hovering over, if any.
    attr_reader   :clicked    # The widget we are clicking on, if any.
    attr_reader   :draggged   # The widget we are currently dragging, if any.
    attr_reader   :focused    # The widget that has the current input focus.  
    attr_reader   :style      # Default style duplicated for all widgets
    attr_reader   :mouse      # Mouse information
    attr_reader   :keyboard   # Keyboard information
    attr_reader   :joysticks  # Array of joystick information
    
    # Overridden self. Needed for some nested initialization problem in Dialog.
    def self.new(*args)
      result   = super(*args)
      @current = result
      result.init     # Post initialization
      return result   # Don't forget to return the result!
    end
    
    # Returns the currently active Hanao system.
    def self.current
      @current ||= nil
      return @current
    end
    
   
    def initialize(screen, queue) 
      @screen         = screen
      @queue          = queue
      @style          = Zori::Style.new()      
      @handlers       = {}
      @done           = false
      @active         = true 
      @hovered        = []  # The widgets we are hovering over, if any.
      @pressed        = nil # The widget we are pressing down on, if any.
      @clicked        = nil # The widget we are clicking on, if any.
      @dragged        = nil # The widget we are currently dragging, if any.
      @focused        = nil # The widget that has the current input focus.  
      @mouse          = Mouse.new(0, 0)
      @keyboard       = Keyboard.new()
      @lastrep        = nil      
      @joysticks      = [ Joystick.new ] 
      @focuscursor    = Sisa::Surface.load_alpha(File.join(IMAGE_DIR, 'cursor', 'joystick_0.png'))
      # Cursor for focusing             
      # Set up handlers by name, from constants  
      for name, klass in Sisa::Event.list do
        handler           = "on_#{name.to_s.downcase}".to_sym
        @handlers[klass]  = handler
      end
      @main           = nil
    end
    
    def init
      @main           = Zori::Mainwidget.new() # Main widget
      return self
    end
   
    
    # Sets the active state of this widget. 
    # Fails if the widget is done (quit requested).
    def active=(a)
      return false if @done
      @active = a
    end
    
    # Delegators for the dimensions of this Hanao's screen
    def_delegators :@screen, :w, :h
    def_delegator  :@screen, :w, :wide
    def_delegator  :@screen, :h, :high
    def_delegator  :@screen, :w, :width
    def_delegator  :@screen, :h, :height    
    
    # Add widgets to the main widget
    def <<(child)
      self.main << child
    end 
    
    
    # Returns all widgets that overlap with the given widget
    def overlapping(widget)
      result = []
      @main.self_and_each_child do |child| 
        next unless child.active?
        result << child if widget.overlap?(child)
      end
      return result
    end
    
    
    # Forces the system to update itself from user input. 
    # Returns self. 
    def update()
      return self unless @active 
      # Do nothing unless active
      event = @queue.poll
      while event
        handler = @handlers[event.class]        
        self.send(handler, event)
        event   = @queue.poll
      end
      repeat_keys # Repeat keypresses
      return self
    end
    
    # Draws the system's widgets to the screen, or to the given surface.
    # Returns self. 
    def draw(surface = nil)      
      surface        ||= @screen
      @main.draw_all(surface) if @main
      @mouse.draw(surface)    if @mouse
      self.draw_focus(surface)
      self.draw_debug(surface)
      return self
    end
    
    ## Focus control
    # Tries to focus the widget. Automatically unfocuses the previous 
    # focus, and sends on_focus_lost and on_focus messages (in that order)
    # Return true if the widget was focused, false if not
    # widget may be nil to unset mouse focus
    def focus(widget)
      if widget 
        return false unless widget.can_focus?
      end   
      # Don' t focus if not needed 
      old_widget    = @focused
      @focused      = widget      
      old_widget.on_focus_lost(widget) if old_widget
      @focused.on_focus(old_widget) if @focused
      return true
    end
    
    def draw_debug(surface)
    end
    
    # Removes focus from the given widget, or from all widgets if widget is nil
    def unfocus(widget=nil)
      return self.focus(nil) if widget.nil?  
      if self.focus?(widget) 
        return self.focus(nil)
      end
      return false
    end
    
    # Returns whether or not the widget has the current focus
    def focus?(widget)
      return self.focused == widget
    end
    
    # Draws the focus cursor next to the focused widget
    def draw_focus(target)
      return false unless self.focused && @focuscursor
      x = self.focused.in_left - (@focuscursor.w)
      y = self.focused.vertical_center - (@focuscursor.h / 2)
      screen.put_bitmap(x, y, @focuscursor)
    end
    
    # Returns the widget under this x and y, or nil if not found.
    def under?(x, y)
      return false unless @main
      return @main.under?(x, y)
    end
    
    # Transforms the unicode code point to a utf-8 encoded text. In particular, 
    # it removes null characters that SDL produces, e.g. for dead keys.
    def cleanup_unicode(event)      
      p "Unicode:", event.sym, event.unicode
      code    = event.unicode
      if code == 0
        text  = ''
      elsif SCANCODE_BUG_WORKAROUND && (code == 194 || code == 195) 
      # XXX: ugly workaround for The 194/195 scancode bug on my Ubutu system.
      # Works fine on Redhat, so, nothing much I can do.
      # Should complain to upstream... but most people don't do i17n aware GUI's 
      # in SDL, I guess :p
        text  = [ event.sym ].pack('U*').force_encoding('UTF-8')        
      else    
        text  = [ code ].pack('U*').force_encoding('UTF-8')
      end
      return text
    end
    
    # Repeats keypresses
    def repeat_keys
      # Prevent useless calls to to_repeat though a global repeat speed. 
      @lastrep ||= Time.now
      return unless (Time.now - @lastrep) > REPEAT_SPEED
      @lastrep   = Time.now
      widget     = self.focused
      for key in @keyboard.to_repeat
        res = send_to_interested(:on_key, key.sym, key.mod, key.text)
      end
    end
    
    # Send events to every widget interested in it
    def send_to_interested(method, *args)
      @main.self_and_each_child do | widget |
        next unless widget.active? # Don't send events to disabled widgets
        next if widget.ignore?(method)        
        res = widget.send(method, *args)
        p res if res
        break if res == :stop
        res
      end
    end
    
    # Send events to every widget interested in it, and under the given coordinates
    def send_to_interested_under(x, y, method, *args)
      @main.self_and_each_child do | widget |
        next if widget.ignore?(method)
        next unless widget.inside?(x, y)
        widget.send(method, *args)  
      end
    end
    
    # Event handlers
    def on_active(event)
    end
    
    # Called when key is pressed
    def on_keydown(event)      
      text    = cleanup_unicode(event)            
      keyboard.press(event.sym, event.mod, text)
      send_to_interested(:on_key_down, event.sym, event.mod, text)      
    end
    
    # Called when key is released
    def on_keyup(event)
      text    = cleanup_unicode(event)
      state   = keyboard.state(event.sym)
      send_to_interested(:on_key_up, event.sym, event.mod, text)
      keyboard.release(event.sym)
    end
    
    # Called when the mouse moves
    def on_mousemotion(event)
      old_x , old_y     = @mouse.x , @mouse.y
      @mouse.move(event.x, event.y)
      old_hovered       = @hovered || []
      @hovered          = @main.all_under?(event.x, event.y)    
      new_hovered       = @hovered - old_hovered
      unhovered         = old_hovered - @hovered
      moving_over       = @hovered - new_hovered      
      unhovered.each    { |widget| widget.on_mouse_out(event.x, event.y, nil) }
      new_hovered.each  { |widget| widget.on_mouse_in(event.x, event.y , nil) }
      moving_over.each  { |widget| widget.on_mouse_move(event.x, event.y)     }
      if @hovered.member?(@pressed)
        #we're above the pressed widget. Drag it
        @pressed.on_mouse_drag(old_x, old_y, event.x, event.y)
      end
      @mouse.under      = @hovered
      # send_to_interested(:on_mouse_move_to, event.x, event.y, old_x, old_y)
    end
    
    # Called when the mouse wheel is scrolled
    def on_mousescroll(event, scroll)
      widget = @hovered.first
      if (widget)
        widget.on_scroll(scroll)
      end
    end
    
    # Called when the mouse button is pressed
    def on_mousebuttondown(event)
      scroll = Mouse.is_scroll?(event.button)
      if scroll # redirect to scrolling
        return self.on_mousescroll(event, scroll)
      end      
      widget = @hovered.first
      p widget
      # Send mouse down to first (topmost) hovered widget only  
      # @hovered should be sorted correctly! 
      if (widget) 
        widget.on_mouse_down(event.x, event.y, event.button)
        @pressed        = widget 
        @mouse.clicked  = @pressed
      end        
      # send_to_interested( :on_mouse_down, event.x, event.y, event.button)
    end
    
    # Called when the mouse button is released
    # Opening up a dialog in the fron somehow prevents the click being 
    # unset correctly  
    def on_mousebuttonup(event)      
      if Mouse.is_scroll?(event.button)
        return # do nothing for scroll second phase.
      end
      widget = @hovered.first
      # Send mouse up to first (topmost) hovered widget only 
      # @hovered should be sorted correctly! 
      if (widget) 
        widget.on_mouse_up(event.x, event.y, event.button)       
      end
      
      if @pressed && (@pressed == widget)
        # Send click event if clicked and released are on the same widget
        widget.on_click(event.x, event.y, event.button)       
      end
      @pressed        = nil
      # Nothing is pressed now.
      @mouse.clicked  = @pressed
      # Let the mouse know about it 
            

      # @hovered.each   { |widget| widget.on_mouse_up(event.x, event.y, event.button) }            
      # send_to_interested(:on_mouse_up, event.x, event.y, event.button)
    end
    
    # Called when the joystick axis is moved
    def on_joyaxis(event)
      p "Joy axis: #{event}"
    end
    
    # Called when the joystick ball moved
    def on_joyball(event)
    end
    
    # Called when the joystick hats is moved
    def on_joyhat(event)
    end
    
    # Called when a joystick button is pressed
    def on_joydown(event)
      p "Joy button down: #{event} #{event.which}"
    end
    
    # Called when a joystick button is released
    def on_joyup(event)
      p "Joy button up: #{event}"
    end
    
    # Called when the system wants to shutdown
    def on_quit(event)
      @done = true
    end
    
    # Called when the system window manager notices a change in this application 
    # window's state  
    def on_syswm(event)
    end
    
    # Called wghen this window is resized
    def on_videoresize(event)
    end
    
    # Called when window is exposed. May not be supported on all platforms.
    def on_videoexpose(event)
    end
    
    
    # Drawing helper functions.
    
    # Draws a border with the style's  color settings 
    def put_border(target, colors, x, y, w, h)
      x += 1 ; y += 1 ; w -= 3 ; h -= 3       
      target.put_rectangle(x, y, w, h,  colors.border)
    end 
    
    # Draws an outset with the given colors and with correct dimensions 
    # for a widget with the given sizes
    def put_outset(target, colors, x, y, w, h)
      put_border(target, colors, x, y, w, h)
      w -= 1 ; h -= 1
      target.line(x    , y    , w     , 0     , colors.highlight)
      target.line(x    , y    , 0     , h     , colors.highlight)
      target.line(x + w, y    , 0     , h     , colors.shadow)
      target.line(x    , y + h, w     , 0     , colors.shadow)
    end
         
    # Draws an inset with the given colors and with correct dimensions 
    # for a widget with the given sizes    
    def put_inset(target, colors, x, y, w, h)
      put_border(target, colors, x, y, w, h)
      w -= 1 ; h -= 1
      target.line(x    , y    , w     , 0     , colors.shadow)
      target.line(x    , y    , 0     , h     , colors.shadow)
      target.line(x + w, y + 1, 0     , h - 1 , colors.highlight)
      target.line(x + 1, y + h, w - 1 , 0     , colors.highlight)
    end
    
    # For debugging
    def inspect
      "<#{self.class.name} h: #{self.h} w: #{self.w}>"
    end
    
      # Utility function for displaying a simple text box dialog
    def ask(message)
      return Zori::Dialog.ask(message)
    end
    
    # Utility function for simple dialog
    def yesno(message)
      return Zori::Dialog.yesno(message)
    end
    
    # Utility function for simple dialog
    def cancelok(message)
      return Zori::Dialog.cancelok(message)
    end
    
    # Utility function for simple dialog
    def alert(message)
      return Zori::Dialog.alert(message)
    end
    
    # Utility function for simple dialog
    def textsplash(message, delay= 3.0)
      return Zori::Dialog.textsplash(message, delay)
    end
    
    # Utility function for a status display
    def statusdisplay(message)
      return Zori::Dialog.status(message)
    end
  
  end
end

