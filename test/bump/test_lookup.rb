# encoding: utf-8
require 'test_helper.rb'
require 'bump'


assert { Bump }
assert { Bump::Lookup }
l = Bump::Lookup.new(64)
assert { l }
assert { l.gridsize == 64 }

class Box 
  attr_accessor :x
  attr_accessor :y
  attr_accessor :w
  attr_accessor :h
  
  def initialize(x, y, h, w)
    @x = x
    @y = y
    @w = w
    @h = h
  end
end
  
b1 = Box.new(70, 60, 80, 90)
b2 = Box.new(20, 30, 83, 94)
b3 = Box.new(220, 230, 77, 66)
l << b1
l << b2
l << b3

assert { l[b1] }
assert { l[b2] }
assert { l[b3] }

assert { l[b1].member?(b1) }
assert { l[b1].member?(b2) }
assert { !l[b1].member?(b3) }
assert { l[b2].member?(b1) }
assert { l[b2].member?(b2) }
assert { !l[b2].member?(b3) }
assert { !l[b3].member?(b1) }
assert { !l[b3].member?(b2) }
assert {  l[b3].member?(b3) }


