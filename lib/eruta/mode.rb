module Eruta
  
  # Programs can be in different modes or states. 
  # In each mode, the display is different and the controls also
  # are acting differently.
  class Mode
    attr_reader :name
    
    def initialize(prog, name=:main)       
      @program = prog 
      @name    = name.to_sym
      @next    = false
      # Some shortcuts for ease of use.
      @game    = @program.game
      @screen  = @program.screen
      @queue   = @program.queue
      @ui      = @program.ui
      @font    = @program.font
    end
    
    # Called when the mode is (re)started.
    def start()
      @next = nil # to prevent locking the mode out.
    end 
    
    # Called when the mode is stopped.
    def stop()
      @next = nil # to prevent locking the mode out.
    end 
    
    # Called when the mode is loaded for the first time.
    # if this returns fale, the load (and the program) will be halted.
    def load(*args)
      return true
    end
    
    # Called on any event that the mode should handle. 
    # bny default this dispatches to the handler handle_#{event.type} 
    def handle(event)
      callback = "handle_#{event.type}".to_sym
      return self.send(callback, event) 
      # if self.respond_to?(callback)
    end
    
    def handle_quit(event)
      puts "Quit!"
      @program.quit!
    end
    
    def handle_active(event)
    end
    
    def handle_mousemove(event)
    end
    
    def handle_mousepress(event)
    end
    
    def handle_mouserelease(event)
    end
    
    
    # Called whenever the screen may be drawn to 
    def render(screen)
      @screen.fill([0,128,0])
    end
    
    # Update mode, should also update program logic.  
    def update
    end
    
    
    
    
    # Ends the current mode. 
    def next_mode!(next_mode = :quit)
      @next = next_mode
    end
    
    # Checks what mode to switch to. Should return the name of the next mode to go 
    # to or :quit to quit the program. Return nil or false to stay in this mode. 
    def next_mode
      n = @next
      # to prevent locking the mode out from returning to itself..
      @next = nil if @next    
      return n
    end
  end
end