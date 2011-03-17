

require 'gari'
require 'nofont'

# Small class to inherit from to run small test apps that use gari.
class Gariapp
  
  # Some commonly used colors
  RED      = Gari::Color.rgb(255,   0,   0)
  ORANGE   = Gari::Color.rgb(255, 128,   0)
  YELLOW   = Gari::Color.rgb(255, 255,   0)
  LIME     = Gari::Color.rgb(128, 255,   0)
  GREEN    = Gari::Color.rgb(0  , 255,   0)
  CYAN     = Gari::Color.rgb(0  , 255, 255)
  BLUE     = Gari::Color.rgb(0  ,   0, 255)
  MAGENTA  = Gari::Color.rgb(0  , 255, 255)
  WHITE    = Gari::Color.rgb(255, 255, 255)
  BLACK    = Gari::Color.rgb(  0,   0,   0)
  

  def initialize(wait = 5.0)
    @game    = Gari::Game.new
    @full    = false
    @screen  = @game.openscreen(640, 480, @full)
    @font    = Nofont.default
    @go      = true
    @timeout = wait
  end
  
  def done!
    @go = false
  end
    
  # Override this, but don't forget to call super!  
  def handle_event(ev)  
    if ev.quit?
      @go = false
    else 
      @start = Time.now
    end  
  end  
    
  def handle_events
    ev = Gari::Event.poll 
    while ev
      self.handle_event(ev)
      ev = Gari::Event.poll
    end
  end
  
  # Override this to do any processing, state updates of physics, etc.
  # This will be called after handling the events but before drawing.
  def update_state
  end
   
  # runs the app.
  def run
    @game.startfps 
    @start = Time.now
    while @go
      handle_events
      update_state
      self.render(@screen)
      @game.update
      @game.nextframe
      # Quit if no events for timeout seconds.
      if @timeout
        @go = false if (Time.now - @start) > @timeout
      end  
    end
  end
  
  # Draws a message on the screen, taking care to draw a background rectangle 
  # for visibility. Draws white text on a blue background by default.
  def draw_puts(x, y, text, fg = WHITE, bg = BLUE)
    w = @font.width_of(text) 
    @screen.fillrect(x, y, w, @font.lineskip, bg) if bg
    @font.draw(@screen, x, y, text, fg)
  end  

  
  # Override this and do your drawing in here. Call super to see FPS output.
  # Probably best called after your rendering.
  def render(screen)
    fpsstr = @game.fps.to_s.split('.')[0]
    draw_puts(@screen.w - 50, 5, "FPS: #{fpsstr}")
  end
  
end