require 'test_helper'
require 'test_widget_helper'

require 'gari'
require 'fimyfi'
require 'zori'

assert { Gari              }
assert { Zori::Slider      }


test_widget_interactively do
  widget  = Zori::Input.new() do |o, why|
    warn "input action"
    p o, o.value, why
  end
  

  assert { widget }
  
  widget
end