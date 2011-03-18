require 'test_helper'
require 'bump'


assert { Bump }
assert { Bump::Thing }

class Try
  include Bump::Thing
end  

t = Try.new(px: 10, py: 20, w:30, h:40)
assert { t.p.x == 10 } 
assert { t.p.y == 20 } 
assert { t.px  == t.p.x } 
assert { t.py  == t.p.y } 
assert { t.x   == -5 } 
assert { t.y   ==  0 } 
assert { t.r.x == 15 }
assert { t.rx  == 15 }
assert { t.r.y == 20 }
assert { t.ry  == 20 }
assert { t.w   == 30 }
assert { t.h   == 40 }




t = Try.new(x: 10, y: 20, w:30, h:40) 
assert { t.p.x == 25 } 
assert { t.p.y == 40 } 
assert { t.px  == t.p.x } 
assert { t.py  == t.p.y } 
assert { t.x   == 10 } 
assert { t.y   == 20 } 
assert { t.r.x == 15 }
assert { t.rx  == 15 }
assert { t.r.y == 20 }
assert { t.ry  == 20 }
assert { t.w   == 30 }
assert { t.h   == 40 }



