# encoding: UTF-8

if $0 == __FILE__
%w{. .. ../lib}.each { |p| $: << p } 
end

require 'test_helper'
require 'gari'
require 'nofont'
require 'gariapp'

assert { Nofont } 
nf, i = nil 
fw, fh = 1.0, 2.0
assert { nf = Nofont.default(fw, fh) }
assert { Nofont.default == Nofont.default }
assert { nf.wscale == fw   }
assert { nf.hscale == fh    }
assert { i = nf.lookup("A")  }
assert { i = nf.lookup(" ")  }


# Check module and class existance
@game    = Gari::Game.new
@full    = false
@screen  = @game.openscreen(640, 480, @full)
YELLOW   = Gari::Color.rgb(255, 255, 0)
ORANGE   = Gari::Color.rgb(255, 191, 0)
BLUE     = Gari::Color.rgb(0, 0, 255)
GREEN    = Gari::Color.rgb(0  , 128, 0)
WHITE    = Gari::Color.rgb(255, 255, 255)
TGREEN   = Gari::Color.rgba(0, 255, 0, 127)
p nf.linehigh
SENTENCE = "The five boxing wizards jump quickly."
wo = nil 
assert { wo = nf.width_of(SENTENCE) } 
assert { wo == 222 }
WO = wo


ASCII = %q{!"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~} 

PRIVATE = " \uEF00\uEF01\uEF02\uEF03\uEF04\uEF05\uEF06\uEF07\uEF08\uEF09"
UNICODE = "äëïöüÿ ÄËÏÖÜŸ áéíóúý ÁÉÍÓÚÝ àèìòùỳ ÀÈÒÙỲ âêîôûŷ ÂÊÎÔÛŶ" + PRIVATE

class NofontTestApp < Gariapp
  def initialize(nf)
    super()
    @nf = nf
    @dirty = false
  end
  
  def handle_event(event)
    super
    @dirty = true
  end
  
  def render(screen)
    if @dirty
      @nf.draw_glyph(@screen, 10, 10, :A, YELLOW)
      @nf.draw(@screen, 10, 50, "A", YELLOW)
      @nf.draw(@screen, 10, 100, ASCII, YELLOW)
      @nf.draw(@screen, 10, 150, SENTENCE, GREEN)
      @nf.draw(@screen, 10 + WO, 150, "a[2] = b ^ c / 4", ORANGE)
      @nf.draw(@screen, 10, 250, UNICODE, WHITE)
      @screen.slab(10, 300, 400, 50, BLUE)
      @nf.blend(@screen, 10, 300, UNICODE, TGREEN)
      @nf.blend(@screen, 10, 320, ASCII, TGREEN)
      @dirty = false
    end  
    # allow fps display
    super(screen)
  end
end

# app = NofontTestApp.new(nf)
# app.run








