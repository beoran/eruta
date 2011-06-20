require 'test_helper'
require 'bump'


assert { Bump }
assert { Bump::Mobile }

class TryMobile
  include Bump::Mobile
end  

t = TryMobile.new(px: 10, py: 20, w:30, h:40)
assert { t.p.x == 10 } 
assert { t.p.y == 20 } 
assert { t.px  == t.p.x } 
assert { t.py  == t.p.y } 
assert { t.x   == -5 } 
assert { t.y   ==  0 } 
assert { t.m   ==  1.0 } 
assert { t.v.x ==  0.0 } 
assert { t.v.y ==  0.0 } 
assert { t.dp.zero?    } 
t.prepare(1.0)
assert { t.dp.zero?    } 

t = TryMobile.new(x: 10, y: 20, w:30, h:40) 
assert { t.p.x == 25 } 
assert { t.p.y == 40 } 
assert { t.px  == t.p.x } 
assert { t.py  == t.p.y } 
assert { t.x   == 10 } 
assert { t.y   == 20 } 
assert { t.m   ==  1.0 } 
assert { t.v.x ==  0.0 } 
assert { t.v.y ==  0.0 } 
assert { t.dp.zero?    } 
t.prepare(1.0)
assert { t.dp.zero?    } 


t.v = Bump.vec(2.0, 3.0)
assert { t.v.x   == 2.0 }
assert { t.v.y   == 3.0 }
time = 1.0
t.prepare(time)
assert { !(t.dp.zero?) }
assert { t.dp.x  == time * t.v.x }
assert { t.dp.y  == time * t.v.y }
oldx = t.p.x
oldy = t.p.y
t.update(time)

assert { t.p.x   == t.dp.x + oldx }
assert { t.p.y   == t.dp.y + oldy }
assert { !(t.dp.zero?) }

t1    = TryMobile.new(x: 10.0, y: 20.0, w:30.0, h:40.0)
t1.v  = Bump.vec(2.0, 3.0)
time  = 1.0
t1.prepare(time)
mass  = 100000
t2    = TryMobile.new(x: 41.0, y: 20.0, w:30.0, h:40.0, m: mass) 
assert { t2.m == mass } 
assert { t1.will_collide?(t2) }
t1.collide_mobile(time, t2)
over = t1.overlap_vector(t2)
puts "#{t1.dp.x} #{t1.dp.y}"
puts "#{t1.newx} #{t1.newy}"

puts "#{over.x} #{over.y}"

overdot    = over.lensq
dprel      = t2.dp - t1.dp
delta      = dprel * (over.dot(dprel) / overdot) 
Kernel.p(dprel.x, dprel.y, delta.x, delta.y)
mratio     = t1.m / (t2.m + t1.m)     
Kernel.p(mratio)
sddp       = delta * mratio
oddp       = delta * (1 - mratio)
Kernel.p(sddp.x, sddp.y, oddp.x, oddp.y)
 
