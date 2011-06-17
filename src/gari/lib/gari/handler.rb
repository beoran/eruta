module Gari
  # A mixin module to help your class handle Gari events. Also helps to keep 
  # track if the application is done (quit requested) and if it is active
  module Handler
    def initialize(queue) 
      @done           = false
      @active         = true
      @queue          = queue
      @handlers       = {}
      for val, sym in  Gari::Event::KIND_TO_TYPE  do
        handler           = "on_#{sym.to_s.downcase}".to_sym
        @handlers[val]    = handler
      end
    end
    
    # checks if the class is done
    def done?
      return @done
    end
    
    # Sets the class to be done
    def done!
      @done = true
    end
    
    # checks if the class is currently active
    def active?
      return @active
    end
    
    # Activates or devactivates the class. 
    # Fails if the class is done (quit requested).
    def active=(a)
      return false if self.done?
      @active = a
    end
    
    # Forces the system to update itself from user input. 
    # Returns self. 
    def handle_events()
      return self unless @active 
      # Do nothing unless active
      event     = @queue.poll
      while event
        handler = @handlers[event.kind]
        self.send(handler, event)
        event   = @queue.poll
      end
      return self
    end
    
    # Event handlers, override these in your class
    
    # Called when the window is avtivated. Does nothing with @active!
    def on_active(event)
    end
    
    # Called when key is pressed
    def on_keydown(event)    
    end
    
    # Called when key is released
    def on_keyup(event)
    end
    
    # Called when the mouse moves
    def on_mousemove(event)
    end
    
    # Called when the mouse wheel is scrolled
    def on_mousescroll(event)
    end
    
    # Called when the mouse button is pressed
    def on_mousepress(event)
    end
    
    # Called when the mouse button is released
    def on_mouserelease(event)      
    end
    
    # Called when the joystick axis is moved
    def on_joyaxis(event)
    end
    
    # Called when the joystick ball moved
    def on_joyball(event)
    end
    
    # Called when the joystick hats is moved
    def on_joyhat(event)
    end
    
    # Called when a joystick button is pressed
    def on_joydown(event)
    end
    
    # Called when a joystick button is released
    def on_joyup(event)
    end
    
    # Called when the system wants to shutdown
    def on_quit(event)
      self.done!
    end
    
    # Called when the system window manager notices a change in this
    # application window's state  
    def on_syswm(event)
    end
    
    # Called when this window is resized
    def on_videoresize(event)
    end
    
    # Called when window is exposed. May not be supported on all platforms.
    def on_videoexpose(event)
    end
  end  
end