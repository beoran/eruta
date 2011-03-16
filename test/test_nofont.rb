# encoding: UTF-8

if $0 == __FILE__
%w{. .. ../lib}.each { |p| $: << p } 
end

require 'test_helper'
require 'gari'
require 'nofont'

assert { Nofont } 
nf, i = nil 
assert { nf = Nofont.default }
assert { nf.wscale == 2.0    }
assert { nf.hscale == 4.0    }
assert { i = nf.lookup("A")  }
assert { i = nf.lookup(" ")  }

# Check module and class existance
@game    = Gari::Game.new
@full    = false
@screen  = @game.openscreen(640, 480, @full)
YELLOW   = Gari::Color.rgb(255, 255, 0)
ORANGE   = Gari::Color.rgb(255, 191, 0)
BLUE     = Gari::Color.rgb(128, 128, 255)
GREEN    = Gari::Color.rgb(0  , 128, 0)
WHITE    = Gari::Color.rgb(255, 255, 255)
p nf.linehigh
sentence = "The five boxing wizards jump quickly."
wo = nil 
assert { wo = nf.width_of(sentence) } 
assert { wo == 222 }


@go = true

ASCII = %q{!"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~} 

UNICODE = "äëïöüÿ ÄËÏÖÜŸ áéíóúý ÁÉÍÓÚÝ àèìòùỳ ÀÈÒÙỲ âêîôûŷ ÂÊÎÔÛŶ"

start = Time.now
while @go
  ev = Gari::Event.poll 
  while ev
    if ev.quit?
      @go = false
    else 
      start = Time.now  
    end
    ev = Gari::Event.poll
  end
  nf.draw_glyph(@screen, 10, 10, :A, YELLOW)
  nf.draw(@screen, 10, 50, "A", YELLOW)
  nf.draw(@screen, 10, 100, ASCII, YELLOW)
  nf.draw(@screen, 10, 150, sentence, GREEN)
  nf.draw(@screen, 10 + wo, 150, "a[2] = b ^ c / 4", ORANGE)
  nf.draw(@screen, 10, 250, UNICODE, WHITE)
  # @screen.text(10, 10, @game.frames.to_s, )
  @game.update
  # Quit if no events for 5 seconds.
  @go = false if (Time.now - start) > 5
end






