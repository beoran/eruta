require 'test_helper'
require 'tini'

assert { Tini } 

txt = %{
map "world map" 10 20 30 do
  layer 1 "this has \" a quote "
    10 20 30 40 50 60 78 80
  end
end map
}

res = nil 
assert { res = Tini.parse(txt) } 

