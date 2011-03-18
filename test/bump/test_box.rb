
require 'test_helper'
require 'bump'

# Check module and class existance
assert { Bump }
assert { Bump::Box }
b1, b2, b3, b4 = nil, nil, nil, nil 


assert { b1 = Bump::Box.new(px: 10, py: 20, w:30, h:40) }
assert { b1.x   == -5 } 
assert { b1.y   ==  0 } 
assert { b1.p.x == 10 } 
assert { b1.p.y == 20 } 
assert { b1.r.x == 15 }
assert { b1.r.y == 20 }
assert { b1.px  == b1.p.x } 
assert { b1.py  == b1.p.y } 
assert { b1.r.x == b1.rx }
assert { b1.r.y == b1.ry }
assert { b1.v.x == b1.vx }
assert { b1.v.y == b1.vy }
assert { b1.v.x == 0  }
assert { b1.v.y == 0  }
assert { b1.w   == 30 }
assert { b1.h   == 40 }


assert { b2 = Bump::Box.new(px: 100, py: 110, w:30, h:40, vx: -2, vy: -2) }












