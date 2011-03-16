

require 'gari'
require 'nofont'

# Small class to inherit from to run small test apps that use gari.
class Gariapp
  
  YELLOW   = Gari::Color.rgb(255, 255, 0)
  ORANGE   = Gari::Color.rgb(255, 191, 0)
  BLUE     = Gari::Color.rgb(0  ,   0, 255)
  GREEN    = Gari::Color.rgb(0  , 128, 0)
  WHITE    = Gari::Color.rgb(255, 255, 255)

  def initialize()
    @game    = Gari::Game.new
    @full    = false
    @screen  = @game.openscreen(640, 480, @full)
    @font    = Nofont.default
    @go      = true
    @timeout = 5.0 
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
   
  # runs the app.
  def run
    @game.startfps 
    @start = Time.now
    while @go
      handle_events
      self.render(@screen)
      @game.update
      @game.nextframe
      # Quit if no events for timeout seconds.
      @go = false if (Time.now - @start) > @timeout
    end
  end
  
  # Override this and do your drawing in here. Call super to see FPS output.
  # Probably best called after your rendering.
  def render(screen)
    @screen.slab(@screen.w - 80, 10, 80, 20, BLUE)
    @nf.draw(@screen, @screen.w - 80, 10, "FPS: #{@game.fps}", WHITE)
  end
  
end