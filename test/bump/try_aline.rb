# interactive, visual test for alline . 
#

$: << '..'
$: << '../../lib'
require 'test_helper'
require 'bump'
require 'gari'

# Check module and class existance
@game    = Gari::Game.new
@full    = false
@screen  = @game.openscreen(640, 480, @full)
YELLOW   = Gari::Color.rgb(255, 255, 0)
ORANGE   = Gari::Color.rgb(255, 191, 0)
BLUE     = Gari::Color.rgb(0  , 0, 127)
GREEN    = Gari::Color.rgb(0  , 127, 0)


@go = true

while @go
  ev = Gari::Event.poll 
  while ev
    if ev.quit?
      @go = false
    end
    ev = Gari::Event.poll
  end
  # @screen.text(10, 10, @game.frames.to_s, )
  @game.update
end






