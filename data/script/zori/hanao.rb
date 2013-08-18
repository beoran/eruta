# encoding: UTF-8
# A Hanao is the environment in which the user interface is displayed 
# and with which it interacts. (A Hanao is the sandal strap of a Zori.)
# Remark on the dision of labour between Hanao and Widget:
# Hanao handles everything that regards or influences different widgets,
# Widget handles everything that only regards or influences one widget.
# Hanao also contains various helper functions for drawing, 
# loading rescoures, etc. 
#
# XXX: Think of a better way to handle modal dialogs. This could mean:
# * Keeping track of who is modal in Hanao.
# * Alternatively, use a ModalPane widget to ensure modality.


module Zori
  class Hanao

    # Constants
    # Virtual Directory under which images are stored
    IMAGE_DIR     = "/image"
    FONT_DIR      = "/font"
    
    # Time mouse button must be down to generate a click
    CLICK_TIME    = 0.5
    # Time keyboard button must be down to generate a repeat
    REPEAT_TIME   = 0.5 
    # Time between repeated keys
    REPEAT_SPEED  = 0.05 
    # Enable workaround for utf scancode bug on Ubuntu
    SCANCODE_BUG_WORKAROUND = true
    # Maybe I'm handling the unicode wrong? Here'swhat the SDL docs say:
#  mod stores the current state of the keyboard modifiers as explained in SDL_GetModState.
# 
# The unicode field is only used when UNICODE translation is enabled with SDL_EnableUNICODE. If unicode is non-zero then this is the UNICODE character corresponding to the keypress. If the high 9 bits of the character are 0, then this maps to the equivalent ASCII character:
# 
# char ch;
# if ( (keysym.unicode & 0xFF80) == 0 ) {
#   ch = keysym.unicode & 0x7F;
# }
# else {
#   printf("An International Character.\n");
# }
    
    # Returns the directory used for the images
    def self.image_dir
      return IMAGE_DIR
    end
    
    # Returns the directory used for the fonts
    def self.font_dir
      return FONT_DIR
    end
    
    # Loads an image (not optimized) 
    def self.load_image(*names)
      fname = [self.image_dir, *names].join('/')
      return Store.load_bitmap(fname)
    end
      
    # Loads a font.
    def self.load_font(name, fsize)
      fname = "#{self.font_dir}/#{name}"
      p fname
      p fsize
      font  = Store.load_ttf_font(fname, fsize)
      warn("Could not open font #{name} in #{fname}") if(!font)
      return font
    end

    attr_reader :state        # State of the Hanao
    attr_reader :can          # Capabilities of the Hanao.
    
    attr_reader   :game       # Game
    attr_reader   :screen     # Screen
    attr_reader   :queue      # Event queue
    attr_accessor :main       # Main widget
    attr_reader   :done       # Is the system requesting quit?   
    alias :done?  :done
    attr_reader   :style      # Default style duplicated for all widgets
    attr_reader   :mouse      # Mouse information
    attr_reader   :keyboard   # Keyboard information
    attr_reader   :joysticks  # Array of joystick information
    attr_reader   :defaultfont# Default font
    
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
   
    def initialize()
      @state          = State.new
      @can            = Capability.new
      @defaultfont    = Hanao.load_font('dejavuserif.ttf', 12)
      @style          = Zori::Style.new(@defaultfont)
      @hovered        = []  # The widgets we are hovering over, if any.
      @pressed        = nil # The widget we are pressing down on, if any
      @clicked        = nil # The widget we are clicking on, if any
      @dragged        = nil # The widget we are currently dragging, if any
      @focused        = nil # The widget that has the current input focus.
      @mouse          = Mouse.new(0, 0)
      @keyboard       = Keyboard.new()
      @lastrep        = nil      
      @joysticks      = [ Joystick.new ] 
      @focuscursor    = Hanao.load_image('ui', 'cursor', 'joystick_0.png')
      # Cursor for focusing             
      @main           = nil
      @modal          = nil # Curent modal widget. 
    end
    
    def init
      @main           = Zori::Mainwidget.new() # Main widget
      return self
    end
      
    # Hanao's screen is for now hard coded to 640x480.
    # This is a virtual resolution as the Eruta system might (later on)
    # implement resolution-independence though scaling.
    def w
      640
    end
    
    # Hanao's screen is for now hard coded to 640x480.
    # This is a virtual resolution as the Eruta system might (later on)
    # implement resolution-independence though scaling.
    def h
      480
    end
    
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
      self.handle_events()
      return self 
    end
    
    # Update the state of the game that hanao is connected to.
    def update_game
      self.game.update
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
    # Return true if the widget was focused, false if not.
    # Widget may be nil to unset mouse focus.
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
      screen.blit(x, y, @focuscursor)
    end
    
    # Returns the widget under this x and y, or nil if not found.
    def under?(x, y)
      return false unless @main
      return @main.under?(x, y)
    end
    
    # Transforms the unicode code point to a utf-8 encoded text. In particular, 
    # it removes null characters that SDL produces, e.g. for dead keys.
    def cleanup_unicode(event)      
      # p "Unicode:", event.sym, event.unicode
      code    = event.unicode
      if code == 0
        text  = ''
      elsif SCANCODE_BUG_WORKAROUND && (code == 194 || code == 195) 
      # XXX: ugly workaround for the 194/195 scancode bug on my Ubutu system.
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
      text    = event.text # cleanup_unicode(event)            
      keyboard.press(event.key, event.mod, text)
      send_to_interested(:on_key_down, event.key, event.mod, text)      
    end
    
    # Called when key is released
    def on_keyup(event)
      text    = event.text
      state   = keyboard.state(event.key)
      send_to_interested(:on_key_up, event.key, event.mod, text)
      keyboard.release(event.key)
    end
    
    # Called when the mouse moves
    def on_mouse_axes(time, x, y, z, w, dx, dy, dz, dw)
      old_x , old_y     = @mouse.x , @mouse.y
      @mouse.move(x, y)
      old_hovered       = @hovered || []
      @hovered          = @main.all_under?(x, y)
      new_hovered       = @hovered - old_hovered
      unhovered         = old_hovered - @hovered
      moving_over       = @hovered - new_hovered
      unhovered.each    { |widget| widget.on_mouse_out(x, y, nil) }
      new_hovered.each  { |widget| widget.on_mouse_in(x, y , nil) }
      moving_over.each  { |widget| widget.on_mouse_move(x, y)     }
      if @hovered.member?(@pressed)
        # Drag if the mouse is hovering above the "pressed" widget.
        @pressed.on_mouse_drag(old_x, old_y, event.x, event.y)
      end
      @mouse.under      = @hovered
      # send_to_interested(:on_mouse_move_to, event.x, event.y, old_x, old_y)
      # scoll mouse too
      on_mouse_scroll(z)
      return true
    end
    
    # Called when the mouse wheel is scrolled
    def on_mouse_scroll(z)
      widget = @hovered.first
      if (widget && z != 0)
        widget.on_scroll(event.value)
      end
    end
    
    # Called when the mouse button is pressed
    def on_mousepress(event)
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
    # XXX: Opening up a dialog in the front somehow prevents the click being 
    # unset correctly  
    def on_mouserelease(event)      
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
      # Nothing is pressed now since button went up.
      @mouse.clicked  = @pressed
      # Let the mouse know about the widget it clicked on
      # @hovered.each   
      # { |widget| widget.on_mouse_up(event.x, event.y, event.button) }
      # send_to_interested(:on_mouse_up, event.x, event.y, event.button)
    end
    
    # Called when the joystick axis is moved
    def on_joyaxis(event)
      p "Joy axis: #{event}"
    end
    
    # Called when a joystick button is pressed
    def on_joydown(event)
      p "Joy button down: #{event} #{event.which}"
    end
    
    # Called when a joystick button is released
    def on_joyup(event)
      p "Joy button up: #{event}"
    end
    
    
    # Handle incoming events
    def on_event(*args)
      type = args.shift
      meth = "on_#{type}".to_sym
      if self.respond_to?(meth)
        return self.send(meth, *args)
      else
        return false
      end
      return false
    end
    
    
    
    # For debugging
    def inspect
      "<#{self.class} h: #{self.h} w: #{self.w}>"
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

