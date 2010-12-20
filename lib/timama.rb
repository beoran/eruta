#
# Timama: a Tile Map Maker for Eruta. 
#
#
require 'gari'
require 'zori'
require 'fimyfi'

class Timama
  # initializes the application
  def initialize
    @game   = Gari::Game.new
    @screen = @game.openscreen(640, 480, false)
    @queue  = Gari::Event
    @ui     = Zori.open(@game, @queue)    
  end
  
  
  # Renders the application and updates the app's state
  def update
    @ui.update
    @ui.draw
    @game.update
  end
  
  # Runs the application.
  def main
    until @ui.done do
      self.update
    end
  end
  

  # Instantiates the application and starts it.  
  def self.main
    @app = self.new
    @app.main
  end

end















