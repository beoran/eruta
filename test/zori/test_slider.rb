require 'test_helper'
require 'test_widget_helper'

require 'gari'
require 'fimyfi'
require 'zori'

assert { Gari              }
assert { Zori::Slider      }


test_widget_interactively do
  slider  = Zori::Slider.new()      { |v| "Slider value changed #{v.value}" }
  slider
end

