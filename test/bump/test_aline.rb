
require 'test_helper'
require 'bump'

# Check module and class existance
assert { Bump }
assert { Bump::Aline }
a1, a2, a3, a4, a5 = nil, nil, nil, nil, nil
include Bump
# Check constructor callable
assert { a1 = Aline.new(0, 1, 1)  }
assert { a2 = Aline.new(4, 1, 0)  }
assert { a3 = Aline.new(1, 2, 0)  }
assert { a5 = Aline.new(0, 2, 1)  }
# Check constructor correctnes
assert { a1.c == 0.0 } 
assert { a2.c == 4.0 }
assert { a3.c == 1.0 }
assert { a1.r == 1.0 }
assert { a2.r == 1.0 }
assert { a3.r == 2.0 }
assert { a1.v == 1.0 }
assert { a2.v == 0.0 }
assert { a3.v == 0.0 }

# Check bump_now?
assert { a1.bump_now?(a3)   }
assert { a3.bump_now?(a1)   }
assert { a1.bump_now?(a3) && a3.bump_now?(a1)   }
assert { !a1.bump_now?(a2)  }
assert { !a2.bump_now?(a1)  }
assert { !a1.bump_now?(a2) && !a2.bump_now?(a1)  }

# Check #step correctness
dt     = 1.0
adt    = []
assert { adt[0] = a1 }  
assert { 10.times { |i| adt[i+1] = adt[i].step(dt) } } 
10.times do |i| 
  assert { adt[i+1].c == adt[i].c + adt[i].v * dt } 
end   

# Check bump_time
bt = nil
assert { bt = a1.bump_time(a2) }
# If bt is 6.0, we're getting the time for collision on the wrong, 
# far side of the aline! 
assert { bt != 6.0 }
# The collision on the right side is at time 2.0 .
assert { bt == 2.0 }
# And vice versa
assert { bt = a2.bump_time(a1) }
# If bt is 6.0, we're getting the time for collision on the wrong, 
# far side of the aline! 
assert { bt != 6.0 }
# The collision on the right side is at time 2.0 .
assert { bt == 2.0 }
# Should be reciprocal.
assert { a1.bump_time(a2)  == a2.bump_time(a1) }
assert { a1.negative_bump_time(a2)  == a2.positive_bump_time(a1) }
p a1.negative_bump_time(a2)
p a1.positive_bump_time(a2)

# Already overlapping should give negative bump times, meaning the 
# initial collision happened in the past.
assert { a1.bump_time(a3) < 0.0 } 
# Check corner cases: objects with same speed will never collide
assert { a3.bump_time(a2).nil? && a3.v == a2.v }
# Check corner cases: objects with same speed will never collide, including self.
assert { a1.bump_time(a1).nil?                 }
assert { a1.bump_c(a2) == 2.0                  }
assert { a5.bump_c(a2) == 1.0                  }
p a1.bump_time(a5) 






