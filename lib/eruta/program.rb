#
module Eruta
  # A generic class for programs that use the same famefor as Eruta
  # I.e. Gari, Zori, Nofont, Fimyfi, etc.
  # Programs support different modes.
  class Program
    # Currently active mode
    attr_reader :mode
    # Still busy?
    attr_reader :busy
    # screen 
    attr_reader :screen
    # ui
    attr_reader :ui
    # event queue 
    attr_reader :queue
    # default font
    attr_reader :font
    # Game info and frames drawn
    attr_reader :game
  
  
    def initialize(screen_w, screen_h, fullscreen)
      @game         = Gari::Game.new
      @screen       = @game.openscreen(screen_w, screen_h, fullscreen)
      @queue        = Gari::Event
      @ui           = Zori.open(@game, @queue)
      fontname      = File.join(*%w{data font liberationserif.ttf})
      @font         = Gari::Font.new(fontname, 8)
      unless @font
        warn "Could not load font #{fontname}."
        @font       = Nofont.default
      end
      @modes        = {} 
      @mode         = nil
      @modename     = nil
      @busy         = true
    end
    
    def add_mode(klass, name, *args)
      m                 = klass.new(self, name.to_sym)
      ok                = m.load(*args)
      return false unless ok
      @modes[m.name]    = m
      return m
    end
    
    def mode=(name)
      return @mode if name.to_sym == @modename
      @modename = name.to_sym
      m         = @modes[name.to_sym]
      raise "No such mode #{name}" unless m
      @mode.stop if @mode # Tell the mode it got stopped if any. 
      @mode     = m
      @mode.start # Tell the mode it got started.
      return @mode
    end
    
    # End the program gracefully.
    def quit!
      @busy = false
    end
    
    
    # Runs the program, switching between active modes
    # A mode must have been installed and selected befiore calling this
    def run
      @game.startfps
      while @busy
        run_once
      end
    end
    
    # Program loop, called once every redraw
    def run_once
      # Poll the events and send them.
      # XXX :coordinate this with the UI.
      # The UI, when active should accept all events it can use, but
      # relay the other ones to the mode in stead of swallowing them.
      event = @queue.poll
      while event do
        # Let the mode handle events.
        @mode.handle(event)
        event = @queue.poll
      end
      # Let the active mode draw to the screen.
      @mode.render(@screen)
      # Update game FPS and screen.  
      @game.update
      # Update mode, should also update program logic, whatever that may be.  
      @mode.update
      # Check if the mode is done. If it is, go to the next mode.
      nextmode = @mode.next_mode
      if nextmode
        if nextmode == :quit 
          self.quit!
        else  
        @mode = nextmode
        end
      end
      # That should be all for now.
    end
  end
end
    
    
    
    
    
  